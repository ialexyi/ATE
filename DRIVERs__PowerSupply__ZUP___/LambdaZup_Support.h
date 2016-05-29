#ifndef __LambdaZup_Support_H__
#define __LambdaZup_Support_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "LambdaZup_Types.h"
#include "LambdaZup.h"

//==============================================================================
// Constants
		
#define		PS_COM_DELAY			0.02
		
#define		OPEN_TIMEOUT			2.0
		
#define		VI_ASRL_BAUD_9600		9600		
#define		VI_ASRL_BITS_8			8
	
#define		FLD_PROTECT_ARM			1
#define		FLD_PROTECT_RELEASE		0
#define		FLD_PROTECT_CANCEL		2		

/* Constants for Wraper */		
#define		NO_ERRORS	0
		
//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

errEV GetZupChannelsQuantity ( int handle , int *pChannelsQuantity );		
		
errEV IntToStringAddress ( int iPSAddress , char *pPSAddressBuf , ViUInt32 *ipBufLength );

errEV DoubleToStringVoltage ( tsLambdaChannel type , double lfVolt , char *pVoltsBuf , ViUInt32 *ipBufLength );
errEV DoubleToStringCurrent ( tsLambdaChannel type , double lfAmpers , char *pVoltsBuf , ViUInt32 *ipBufLength );
errEV DoubleToStringOverProtection ( tsLambdaChannel type , double lfVolts , char *pVoltsBuf , ViUInt32 *ipBufLength );
errEV DoubleToStringUnderProtection ( tsLambdaChannel type , double lfVolts , char *pVoltsBuf , ViUInt32 *ipBufLength );


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __LambdaZup_Support_H__ */
