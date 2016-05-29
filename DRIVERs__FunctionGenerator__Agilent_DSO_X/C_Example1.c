#include <stdio.h>
#include "AgInfiniiVision.h"
// Link to: <Program Files>\IVI Foundation\IVI\Lib\msc\AgInfiniiVision.lib

///
/// Agilent IVI-C Driver Example Program
/// 
/// Initializes the driver, reads a few Identity interface
/// properties, and checks the instrument error queue.
/// May include additional instrument specific functionality.
/// 
/// See driver help topic "Programming with the IVI-C Driver in Various Development Environments"
/// for additional programming information.
///
/// Runs in simulation mode without an instrument.
/// 

void main()
{
	ViStatus status;
	ViSession session;
	ViChar str[128];
	ViInt32 ErrorCode;
	ViChar ErrorMessage[256];
	ViBoolean simulate;

	ViReal64 WaveformArray[256] = {0.0};
	ViInt32 WaveformSize = 25;
	ViReal64 InitialX = 0.0;
	ViReal64 XIncrement = 0.0;
    ViInt32 ActualNumberOfPoints = 0;
	ViInt32 i = 0;

	// Edit resource and options as needed.  resource is ignored if option Simulate=true
	char resource[] = "GPIB0::7::INSTR";
	//char resource[] = "TCPIP0::<ip or host name>::INSTR";
	
	char options[]  = "QueryInstrStatus=true, Simulate=true, DriverSetup= Model=, Trace=false";

	ViBoolean idQuery = VI_TRUE;
	ViBoolean reset   = VI_TRUE;

	printf("  C_Example1\n\n");

	// Initialize the driver.  See driver help topic "Initializing the IVI-C Driver" for additional information
	status = AgInfiniiVision_InitWithOptions(resource, idQuery, reset, options, &session);
	if(status)
	{
		// Initialization failed
		AgInfiniiVision_GetError(session, &ErrorCode, 255, ErrorMessage);
		printf("** InitWithOptions() Error: %d, %s\n", ErrorCode, ErrorMessage);
		printf("\nDone - Press Enter to Exit");
		getchar();  
		return;
	}
	printf("Driver Initialized \n");

	// Read and output a few attributes
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_SPECIFIC_DRIVER_PREFIX, 127, str);
	printf("DRIVER_PREFIX:      %s\n", str);
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_SPECIFIC_DRIVER_REVISION, 127, str);
	printf("DRIVER_REVISION:    %s\n", str);
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_SPECIFIC_DRIVER_VENDOR, 127, str);
	printf("DRIVER_VENDOR:      %s\n", str);
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_SPECIFIC_DRIVER_DESCRIPTION, 127, str);
	printf("DRIVER_DESCRIPTION: %s\n", str);
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_INSTRUMENT_MODEL, 127, str);
	printf("INSTRUMENT_MODEL:   %s\n", str);
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_INSTRUMENT_FIRMWARE_REVISION, 127, str);
	printf("FIRMWARE_REVISION:  %s\n", str);
	status = AgInfiniiVision_GetAttributeViString(session, "", AGINFINIIVISION_ATTR_SYSTEM_SERIAL_NUMBER, 127, str);
	printf("SERIAL_NUMBER:      %s\n", str);
	status = AgInfiniiVision_GetAttributeViBoolean(session, "", AGINFINIIVISION_ATTR_SIMULATE, &simulate);
	if (simulate == VI_TRUE)
		wprintf(L"\nSIMULATE:           True\n\n");
	else
		wprintf(L"SIMULATE:           False\n\n");


	// TODO: Exercise driver methods and properties
	//Initiate the acquisition and return the acquired waveform data
	status = AgInfiniiVision_MeasurementsAutoSetup(session);
	status = AgInfiniiVision_MeasurementsInitiate(session);
	status = AgInfiniiVision_MeasurementFetchWaveform(session,"Channel1", WaveformSize, &WaveformArray, &ActualNumberOfPoints, &InitialX, &XIncrement);
	printf("ActualNumberOfPoints, InitialX, XIncrement: %d,%.15g,%.15g\n\n", ActualNumberOfPoints, InitialX, XIncrement);
    
	//Print out the first ten values 
	printf("Waveform Data: \n\n");
	for ( i =0; i< 10; i++)
		printf("\t%le\n\n",WaveformArray[i]);


	// Check instrument for errors
	ErrorCode = 1;
	printf("\n");
	while(ErrorCode)
	{
		status = AgInfiniiVision_error_query( session, &ErrorCode, ErrorMessage);
		printf("error_query: %d, %s\n", ErrorCode, ErrorMessage);
	}

	// Close the driver
	status = AgInfiniiVision_close(session);
	if(status)
	{
		AgInfiniiVision_GetError(session, &ErrorCode, 255, ErrorMessage);
		printf("** Close() Error: %d, %s\n", ErrorCode, ErrorMessage);
	}
	else
		printf("Driver Closed \n");

	printf("\nDone - Press Enter to Exit");
	getchar();  
}

