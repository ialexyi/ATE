#ifndef __DaqMx_Driver_H__
#define __DaqMx_Driver_H__

//==============================================================================
// Include files

#include "cvidef.h"
#include "GlobalDefines.h" 

//==============================================================================
// Constants

//==============================================================================
// Types

typedef enum { DIGITAL_CONNECT_1_BIT , DIGITAL_CONNECT_8_BIT , DIGITAL_CONNECT_16_BIT , DIGITAL_CONNECT_32_BIT , DIGITAL_CONNECT_40_BIT , DIGITAL_CONNECT_48_BIT , DIGITAL_CONNECT_64_BIT , DIGITAL_CONNECT_SENTINEL } teDigitalConnectionSizeType;

typedef enum { DAQMX_CONFIG_TYPE_EMPTY , DAQMX_CONFIG_TYPE_ANALOG_CURRENT_IN , DAQMX_CONFIG_TYPE_ANALOG_VOLT_IN , DAQMX_CONFIG_TYPE_ANALOG_VOLT_OUT , DAQMX_CONFIG_TYPE_DIGITAL , DAQMX_CONFIG_TYPE_COUNTER , DAQMX_CONFIG_TYPE_SENTINEL } teDAQMX_ConfigTypes;

typedef struct
{
	unsigned __int64			UseBitsMap,
								BitDirectionMask,
								DefValue;
	
	char						bPowerUpState,
								bPullingState;
	
	double						dPowerUpLevel;
	
	double						lfMinValue,
								lfMaxValue;
	
	int							iMeasureType;
	
	int							*pPortNumbersArray,
								iPortArraySize;
	
	char						szProtect[64];
	
} tsDaqmxPortConfigItem;
//==============================================================================
// External variables

//==============================================================================
// Global functions
int 	DLLEXPORT 	DAQMX_GetErrorMessage( int error , char *pszDescription );

void*	DLLEXPORT	DAQMX_Close(int *pHandle);
void*	DLLEXPORT	DAQMX_Init(teDAQMX_ConfigTypes type,char *pszDeviceName, tsDaqmxPortConfigItem ConfigBuffer, int *pHandle);
void*	DLLEXPORT	DAQMX_SetDigitalBit(int Handle,unsigned char bState);
void*	DLLEXPORT	DAQMX_GetDigitalBit(int Handle,unsigned char *pbState);
void*	DLLEXPORT	DAQMX_SetPort(int Handle,unsigned char byte);
void*	DLLEXPORT	DAQMX_GetPort(int Handle,unsigned char *pByte);
void*	DLLEXPORT	DAQMX_SetAnalogLine(int Handle,double value);
void*	DLLEXPORT	DAQMX_GetAnalogLine(int Handle,double *pGetValue);
void*	DLLEXPORT	DAQMX_GetAnalogLineAvarage( int Handle , unsigned int number , double *pGetValue ); 
void*	DLLEXPORT	DAQMX_GetDigitalPulseCharacters( int Handle , int numberOfSimples , double lfTimeout , double *plfAmplitude , double *plfWidth , double *plfStart );

void*	DLLEXPORT	DAQMX_ConfigSampleRate( int Handle , double lfSampleRate ); 
void*	DLLEXPORT	DAQMX_DoneTask( int Handle ); 
	
void*	DLLEXPORT	DAQMX_GetDigitalBitArray( int Handle , int numberOfSimples , double lfTimeout , unsigned char **pArrayValue , int *pCount ); 

void*	DLLEXPORT	DAQMX_SetAnalogLineWave( int Handle , int bContinues , double lfFrequency , unsigned int number , double *plfArray ); 
void*	DLLEXPORT	DAQMX_GetAnalogLineWave( int Handle , double lfTimeOut , unsigned int number , unsigned long long SampsPerChannel , double **plfArray ); 

void*	DLLEXPORT	DAQMX_CalculateTHD( int Handle , double lfTimeOut , unsigned int number , double lfSampleRate , double lfHarmonicFrequency , double *plfPercent_THD , double *plfPercent_THDNoise ); 

void*	DLLEXPORT	DAQMX_SetUniversal( int Handle , unsigned __int64 iValue ); 
void*	DLLEXPORT	DAQMX_GetUniversal( int Handle , unsigned __int64 *piValue );

void*	DLLEXPORT	DAQMX_GetValueType( int Handle , teDigitalConnectionSizeType *pSizeType );

#endif  /* ndef __DaqMx_Driver_H__ */
