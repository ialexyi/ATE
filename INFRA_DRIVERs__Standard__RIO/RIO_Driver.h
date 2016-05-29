#ifndef __RIO_Driver_H__
#define __RIO_Driver_H__  

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

void*  RIO_Get_Low_Level_Handle( int hHandle , int *phLowLevelHandle );

void*  RIO_Get_Error_Text_Message_Low_Level_Handle( int hLowLevelHandle , int iError , char *pErrorMessage );
void*  RIO_Get_Error_Text_Message_Instrument_Handle( int hInstrumentHandle , int iError , char *pErrorMessage );   

void*  RIO_Get_Device_Info_Low_Level_Handle ( int hLowLevelHandle, char **pModel , char **pFirmware );
void*  RIO_Get_Device_Info_Instrument_Handle ( int hInstrumentHandle, char **pModel , char **pFirmware ); 

void*  RIO_Init ( int hParentInstrumentHandle , char* szRsrcAddress , int *vhInstrumentHandle ); 
void*  RIO_Close ( int *vhInstrumentHandle );

void*  RIO_Reset ( int hInstrumentHandle ); 

void*  RIO_Get_AI_Line_Value ( int hInstrumentHandle, unsigned char ucLineNumber, double *dResult ); 
void*  RIO_Get_AO_Line_Value  ( int hInstrumentHandle, unsigned char ucLineNumber, double *dResult ); 
void*  RIO_Get_DI_Line_Value  ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult ); 
void*  RIO_Get_DO_Line_Value  ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult ); 

void*  RIO_Set_AO_Line_Value  ( int hInstrumentHandle, unsigned char ucLineNumber, double dValue );
void*  RIO_Set_DO_Line_Value  ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char ucValue );

void*  RIO_Get_AI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult );
void*  RIO_Get_DI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char *ucResult ); 

void*  RIO_Set_AI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char ucRangeType, unsigned char ucIsDifferential); 
void*  RIO_Set_DI_Line_Config ( int hInstrumentHandle, unsigned char ucLineNumber, unsigned char ucIsInvert ); 

unsigned char IsCommandSuccessful(void);  

#endif  /* ndef __RIO_Driver_H__ */
