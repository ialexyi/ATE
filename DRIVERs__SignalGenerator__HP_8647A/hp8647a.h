/*= <HP 8647A SIGNAL GENERATOR> Include File ==============================*/

#ifndef __Hp8647a_HEADER
#define __Hp8901a_HEADER

/*=========================================================================*/
/*  Please note that the use of global variables or arrays in the Include  */
/*  file is no longer allowed.                                             */
/*=========================================================================*/

#define PUBLIC __stdcall

#if defined (__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*== GLOBAL CONSTANT DECLARATIONS =========================================*/

/* Replace 10 with the maximum number of devices of this type being used.  */
#define hp8647a_MAX_INSTR    10

/*== GLOBAL FUNCTION DECLARATIONS =========================================*/

int PUBLIC hp8647a_init (int GPIBAddress, int IDQuery, int resetDevice, int *instrumentID);
int PUBLIC hp8647a_output_signal (int instrumentID, double RFFrequencyMHz,
                           double RFAmplitudedBm, int modulation,
                           double modulationLevel, int modulatingSignal,
                           int internalFrequency, int externalCoupling);
int PUBLIC hp8647a_rf_signal (int instrumentID, double RFFrequencyMHz,
                       int amplitudeReference, double referenceValuedBm,
                       double RFAmplitudedBm, int automaticAttenuator);
int PUBLIC hp8647a_modulation (int instrumentID, int modulation, double modulationLevel,
                        int modulatingSignal, int internalFrequency,
                        int externalCoupling);
int PUBLIC hp8647a_gen_signl (int instrumentID, int modulationOFF, int RFOutputSignal);
int PUBLIC hp8647a_write (int instrumentID, char writeBuffer[]);
int PUBLIC hp8647a_read (int instrumentID, int numberBytesToRead, char readBuffer[],
                  int *numBytesRead);
int PUBLIC hp8647a_reset (int instrumentID);
int PUBLIC hp8647a_self_test (int instrumentID, int *passFailCode, char selfTestMessage[]);
int PUBLIC hp8647a_revision_query (int instrumentID, char driverRevision[],
                            char firmwareRevision[]);
int PUBLIC hp8647a_set_ref_att (int instrumentID, int amplitudeReference,
                         int automaticAttenuator);
int PUBLIC hp8647a_det_mod_set (int selectedModulation, double desiredModulValue,
                         double extModSignalLevel, double *valueToSetModul);
int PUBLIC hp8647a_mod_on_off (int instrumentID, int selectModulation, int modulation);
int PUBLIC hp8647a_close (int instrumentID);

#if defined (__cplusplus) || defined(__cplusplus__)
}
#endif

#endif
