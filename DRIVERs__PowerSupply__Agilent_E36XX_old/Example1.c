// The driver front panel file,
// <Program Files>\IVI Foundation\IVI\Drivers\AgilentE36xx\AgE36xx.fp must
// be loaded and included in your CVI project

// The #include for the driver should be the first #include to
// prevent potential conflicts with NI libraries.
#include "AgE36xx.h"
#include <stdio.h>

void main()
{
	ViStatus status;
	ViSession session;
	ViChar str[128];
	ViInt32 ErrorCode;
	ViChar ErrorMessage[256];
	ViReal64 measVal;

	// Edit resource and options as needed
	char resource[] = "GPIB0::5::INSTR";
//	char resource[] = "TCPIP0::127.0.0.1::INSTR";
	
	// resource is ignored if Simulate=true
	char options[]  = "Simulate=true";
	
	ViBoolean idQuery = VI_TRUE;
	ViBoolean reset   = VI_TRUE;

	// Initialize the driver
	status = AgE36xx_InitWithOptions(resource, idQuery, reset, options, &session);
	if(status)
	{
		// Initialization failed
		AgE36xx_GetError(session, &ErrorCode, 255, ErrorMessage);
		printf("** Init() Error: %d, %s\n", ErrorCode, ErrorMessage);
		printf("\nDone - Press Enter to Exit");
		getchar();  
		return;
	}
	printf("Driver Initialized\n");

	// Read and output a few attributes
	status = AgE36xx_GetAttributeViString(session, "", AGE36XX_ATTR_SPECIFIC_DRIVER_DESCRIPTION, 127, str);
	printf("DRIVER_DESCRIPTION:\n  %s\n", str);
	status = AgE36xx_GetAttributeViString(session, "", AGE36XX_ATTR_SPECIFIC_DRIVER_REVISION, 127, str);
	printf("DRIVER_REVISION:   %s\n", str);
	status = AgE36xx_GetAttributeViString(session, "", AGE36XX_ATTR_INSTRUMENT_MODEL, 127, str);
	printf("INSTRUMENT_MODEL:  %s\n", str);
	status = AgE36xx_GetAttributeViString(session, "", AGE36XX_ATTR_INSTRUMENT_FIRMWARE_REVISION, 127, str);
	printf("FIRMWARE_REVISION: %s\n\n", str);
	
	
	// TODO: Driver specific tasks

	// Set output voltage.
	status = AgE36xx_SetAttributeViReal64(session, "Output1", AGE36XX_ATTR_VOLTAGE_LEVEL, 1.23);
	printf("Output 1 set to: 1.23 Volts\n");

	// Enable all outputs.
	status = AgE36xx_SetAttributeViBoolean(session, "", AGE36XX_ATTR_OUTPUTS_ENABLED, VI_TRUE);
	printf("All outputs enabled\n");

	// Measure the output voltage.
	status = AgE36xx_Measure(session, "Output1", AGE36XX_VAL_MEASURE_VOLTAGE, &measVal);
	printf("Output 1 Measurement = %f Volts\n", measVal);

	// Disable all outputs.
	status = AgE36xx_SetAttributeViBoolean(session, "", AGE36XX_ATTR_OUTPUTS_ENABLED, VI_FALSE);
	printf("All outputs disabled\n");

	// Measure the output voltage.
	status = AgE36xx_Measure(session, "Output1", AGE36XX_VAL_MEASURE_VOLTAGE, &measVal);
	printf("Output 1 Measurement = %f Volts\n", measVal);

	
	// Read instrument error queue until empty.
	ErrorCode = 1;
	printf("\n");
	while(ErrorCode)
	{
		status = AgE36xx_error_query( session, &ErrorCode, ErrorMessage);
		printf("error_query: %d, %s\n", ErrorCode, ErrorMessage);
	}

	// Close the driver
	status = AgE36xx_close(session);
	if(status)
	{
		AgE36xx_GetError(session, &ErrorCode, 255, ErrorMessage);
		printf("** Close() Error: %d, %s\n", ErrorCode, ErrorMessage);
	}
	else
		printf("Driver Closed\n");


	printf("\nDone - Press Enter to Exit");
	getchar();  
}

