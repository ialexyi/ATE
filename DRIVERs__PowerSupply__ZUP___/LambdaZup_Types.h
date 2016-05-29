#ifndef __LambdaZup_TYPES_H__
#define __LambdaZup_TYPES_H__

//==============================================================================
// Types

/***************************************************/
typedef struct 
{
	double						lfMaxValue,
								lfMinValue,
								lfAccuracy;
	
	char						szProtect[64];
} tsTypeItem;

/***************************************************/

typedef struct 
{
	char						szPowerSupplyModel[32];
	
	tsTypeItem					currentDefine,
								voltageDefine,
								overVoltageDefine,
								underVoltageDefine;

	char						szProtect[64];
	
} tsLambdaTypes;

/***************************************************/

//==============================================================================
// Global functions

int LambdaZupTypesQuantity( void );

#endif  /* ndef __LambdaZup_TYPES_H__ */
