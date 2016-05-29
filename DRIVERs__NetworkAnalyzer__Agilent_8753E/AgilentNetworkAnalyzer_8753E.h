#ifndef __AgilentNetworkAnalyzer_8753E_H__
#define __AgilentNetworkAnalyzer_8753E_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants
#define		NETWORK_ANALYZER_NUMBER_OF_PORTS	2
#define		NETWORK_ANALYZER_PORT11				0
#define		NETWORK_ANALYZER_PORT22				1

#define		NETWORK_ANALYZER_S_PAR21			7
#define		NETWORK_ANALYZER_S_PAR11			6 
#define		NETWORK_ANALYZER_S_PAR22			9 
#define		NETWORK_ANALYZER_CONVER_NO			0
	 
#define		NETWORK_ANALYZER_NO					"NO"
#define		NETWORK_ANALYZER_YES				"YES"
#define		NETWORK_ANALYZER_SET           		"SET"
#define		NETWORK_ANALYZER_PWR_LEVEL        	"LEVEL" 

#define		NETWORK_ANALYZER_CH1				0			
#define		NETWORK_ANALYZER_DATA_TYPE_DDM		3	 // Data divided by memory
#define		NETWORK_ANALYZER_DATA_TYPE_DATA		0
#define		NETWORK_ANALYZER_DATA_TYPE_MEM		1

#define		NETWORK_ANALYZER_DATA_FORMAT_LM		0	 // Log magnitude
#define		NETWORK_ANALYZER_FREQ_TYPE_LF		0    // Linear frequency

#define		NETWORK_ANALYZER_PORT_NUM1			0

#define		NETWORK_ANALYZER_RANGE_AUTO			12
	
#define		MAXIMUM_GET_NA_BUFFER				2000

#define		NETWORK_ANALYZER_OUTPCALC			0	// - Calibration arrays
													//  (error-correction coefficients).  Possible array ranges are 1 - 12.

#define		NETWORK_ANALYZER_OUTPICAL			1	// - Interpolated
													//	calibration arrays (will only be present if interpolation has
													//	been turned ON and stimulus settings have changed in a valid
													//	manner).  Possible array ranges are 1 - 12.


#define		NETWORK_ANALYZER_OUTPDATA			2	//- Error-corrected measurement
													//  trace data as real/imaginary pairs.  

#define		NETWORK_ANALYZER_OUTPDATF 			3	//- Same as OUTPDATA, except
													// will provide faster data transfer under certain conditions.

#define		NETWORK_ANALYZER_OUTPFORM 			4	//- Measurement trace data in
													//	the format that is currently being displayed on the instrument.
													//	If data being displayed is scalar values (i.e., display is
													//	neither Polar nor Smith Chart), the data must still be
													//	transferred as pairs (the second value in each pair is a
													//	meaningless "placeholder").  

#define		NETWORK_ANALYZER_OUTPFORF 			5	//- Same as OUTPFORM, except
													//	that only the desired values will be output (no placeholders)
													//	when the display is in one of the scalar formats, and will
													//	provide faster data transfer under certain conditions when
													//	display format is complex (Polar or Smith Chart).  


#define		NETWORK_ANALYZER_OUTPMEMO			6	// - Memory trace data as
													//   real/imaginary pairs.  

#define		NETWORK_ANALYZER_OUTPMEMF 			7	//- Same as OUTPMEMO, except
													//	will provide faster data transfer under certain conditions.
													//	Parameter readTraceOrCalArrayNumber is ignored.  NOTE:  If no
													//	memory trace is stored in the instrument, running this option
													//	may cause the GPIB bus to lock up.  The user may need to do a
													//	manual preset to clear the error.
		
		
		
#define		NETWORK_ANALYZER_OUTPPREn 			8	//	"Pre-raw" data; Raw
													//	measurement data that has had neither sampler correction nor
													//	attenuator offsets applied to it. A full 2-port cal must be
													//	active.  See Example 2G on how to set up the instrument before
													//	using this driver option.  Note that parameter
													//	readTraceOrCalArrayNumber range is as follows:
													//	        array 1 holds S11 data
													//	        array 2 holds S21 data
													//	        array 3 holds S12 data
													//	        array 4 holds S22 data
													//
													//	Note:  Reading of pre-raw data is only supported on analyzers
													//	having Firmware Revision 6.12 or higher, and is not supported on
													//	8753ET/8719ET/8720ET/8722ET analyzers.

#define		NETWORK_ANALYZER_OUTPRAWn 			9	//	Raw measurement data.
													//	Normally, array number 1 holds the current parameter.  Note that
													//	if a 2-port calibration is active then parameter
													//	readTraceOrCalArrayNumber range is as follows:
													//        	array 1 holds S11 data
													//        	array 2 holds S21 data
													//        	array 3 holds S12 data
													//        	array 4 holds S22 data
													//
#define		NETWORK_ANALYZER_OUTPRAFn			10	//	Same as OUTPRAWn, except
													//	will provide faster data transfer under certain conditions.
													//	Note that if a 2-port calibration is active then parameter
													//	readTraceOrCalArrayNumber range is as follows:
													//        	array 1 holds S11 data
													//			array 2 holds S21 data
													//			array 3 holds S12 data
													//			array 4 holds S22 data
													//  		
		
		
		

#define		NETWORK_ANALYZER_MARKER1			0
#define		NETWORK_ANALYZER_MARKER2			1
#define		NETWORK_ANALYZER_MARKER3			2

#define		NETWORK_ANALYZER_MARKER_ON			"ON"
#define		NETWORK_ANALYZER_MARKER_OFF			"OFF"
#define		NETWORK_ANALYZER_MARKER_MIN			"MINIMUM"
#define		NETWORK_ANALYZER_MARKER_MAX			"MAXIMUM"
#define		NETWORK_ANALYZER_MARKER_DELTA		"DELTA TO FIXED"

#define		NETWORK_ANALYZER_MARKER_GET_MIN_MAX	"VALUE STIM?"

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __AgilentNetworkAnalyzer_8753E_H__ */
