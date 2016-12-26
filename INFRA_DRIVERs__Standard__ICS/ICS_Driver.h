#ifndef __ICS_Driver_H__
#define __ICS_Driver_H__

//==============================================================================
// Include files

#include "cvidef.h"
#include "GlobalDefines.h"

//==============================================================================
// Constants
		
//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

void*	ICS_GetLowLevelHandle( int hHandle , int *phLowLevelHandle );

void* 	ICS_GetErrorTextMessage(int hInstrumentHandle, int iError, char *pErrorMessage);
void* 	ICS_Init( int hParentInstrumentHandle , char* szRsrcAddress , int *vhInstrumentHandle ); 
void* 	ICS_Close(int *vhInstrumentHandle);
void* 	ICS_Reset(int hInstrumentHandle);
void* 	ICS_SetPortConfiguration(int Handle, unsigned char ucPortNumber, unsigned char ucPortPolarity);
void*   ICS_SetPort( int Handle , unsigned char ucPortNumber , unsigned char ucMask , unsigned char ucSetByte , unsigned char ucPortDefaultValue );
void* 	ICS_GetPort(int Handle, unsigned char ucPortNumber, unsigned char *pGetByte);
void* 	ICS_SetBit(int Handle, unsigned char ucPortNumber, unsigned char ucBitIndex, unsigned char ucSetBit);
void* 	ICS_GetBit(int Handle, unsigned char ucPortNumber, unsigned char ucBitIndex, unsigned char *pGetBit);

void*	ICS_SetPortByBit( int Handle , unsigned char ucPortNumber , unsigned char ucMask , unsigned char ucSetByte );
void*	ICS_GetPortByBit( int Handle , unsigned char ucPortNumber , unsigned char ucMask , unsigned char *pGetByte );

void* 	ICS_SPI(int Handle, unsigned char ucPortNumber, unsigned char ucEnableBitIndex, unsigned char ucEnableBitPolarity, unsigned char ucClockBitIndex, unsigned char ucClockBitPolarity, unsigned char ucClockBitPhase, unsigned char ucDataBitIndex, unsigned long long ullSetData, unsigned char ucSetNumberOfBits, unsigned char ucReadBitPortIndex, unsigned char ucReadBitIndex, unsigned long long *ullGetData, unsigned char ucGetNumberOfBits);

void*   ICS_SaveDefaultState( int Handle );


#endif  /* ndef __ICS_Driver_H__ */
