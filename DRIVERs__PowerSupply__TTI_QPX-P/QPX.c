#include <ansi_c.h>
#include <utility.h>
/***************************************************************************** 
 *	  Copyright 2008 Product Technology Partners Ltd.  All Rights Reserved.  * 
 *****************************************************************************/

/*****************************************************************************
 *  Thurlby-Thandar QPX Series PSU Instrument Driver                               
 *  LabWindows/CVI 5.0 Instrument Driver                                     
 *  
 *	Based on nPLP driver by													*
 *  
 *  Product Technology Partners Ltd, www.ptpart.co.uk                              
 *  PH. +44 01954212806   Fax +44 01954212808                              
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *  Converted by PH at TTi, January 2009     	
 
 *	Inital Release  v1.0
 
 
 *  v1.1 	August 		2010		Added support for QPX600   
 				
 *  v1.1.1  September   2010		Corrected Link mode operation on QPX600DP
 
 *	v1.2    September   2012  		Fixed virtual channel name coercion
 
 * 	v1.3    July	    2014        Fixed QPX_ATTR_ID_QUERY_RESPONSE in simulation mode     
 
 *  v1.3.3  August		2014		Fixed QueryOutputState function
 
 *****************************************************************************/

#include <userint.h>
#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include <ctype.h>
#include "QPX.h"

static ViBoolean VerifyVoltage = VI_FALSE;
PSUModel_t PSUModel = QPX1200;
/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define QPX_ATTR_OPC_TIMEOUT      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)   /* ViInt32 */

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        
#define QPX_IO_SESSION_TYPE   IVI_VAL_VISA_SESSION_TYPE

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT   0x08
#define IEEE_488_2_EXECUTION_ERROR_BIT          0x10
#define IEEE_488_2_COMMAND_ERROR_BIT            0x20

    /*- List of channels passed to the Ivi_BuildChannelTable function -------*/ 
//#define CHANNEL_LIST                            "1, 2"	 //default to one, modified by QPX_IviInt
#define CHANNEL_LIST                            "1,2"	 //default to two

static IviRangeTableEntry attrTrackingModeRangeTableEntries[] =
	{
		{0, 0, 0, "", 0},
		{1, 0, 0, "", 0},
		{2, 0, 0, "", 0},
		{3, 0, 0, "", 0},
		{4, 0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrTrackingModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTrackingModeRangeTableEntries,
	};

static IviRangeTableEntry attrRatioRangeTableEntries[] =
	{
		{5, 2000, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrRatioRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrRatioRangeTableEntries,
	};





static IviRangeTableEntry attrAddressRangeTableEntries[] =
	{
		{1, 255, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAddressRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAddressRangeTableEntries,
	};

static IviRangeTableEntry attrOpcTimeoutRangeTableEntries[] =
	{
		{0, 2147483648, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrOpcTimeoutRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrOpcTimeoutRangeTableEntries,
	};

static IviRangeTableEntry attrStatusRegisterRangeTableEntries[] =
	{
		{0, 2147483648, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrStatusRegisterRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrStatusRegisterRangeTableEntries,
	};

static IviRangeTableEntry attrOCPLimitRangeTableEntries[] =
	{
		{2, 55, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrOCPLimitRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrOCPLimitRangeTableEntries,
	};

static IviRangeTableEntry attrRangeSettingRangeTableEntries[] =
	{
		{1, 0, 0, "", QPX_VAL_RANGE_1},
		{2, 0, 0, "", QPX_VAL_RANGE_2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrRangeSettingRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrRangeSettingRangeTableEntries,
	};

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus QPX_InitRangeTables (ViSession vi);
static ViStatus QPX_InitAttributes (ViSession vi);
static ViStatus QPX_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus QPX_CheckStatus (ViSession vi);
static ViStatus QPX_WaitForOPC (ViSession vi, ViInt32 maxTime);
static ViStatus QPX_GetDriverSetupOption (ViSession vi,
                                               ViConstString optionTag,
                                               ViInt32 optionValueSize,
                                               ViChar optionValue[]);

    /*- File I/O Utility Functions -*/
static ViStatus QPX_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus QPX_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC QPX_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC QPX_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC QPXAttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC QPXAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC QPXAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC QPXAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC QPXAttrOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);
static ViStatus _VI_FUNC QPXAttrVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC QPXAttrVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);
static ViStatus _VI_FUNC QPXAttrOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value);
static ViStatus _VI_FUNC QPXAttrOVPLimit_ReadCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64* value);
static ViStatus _VI_FUNC QPXAttrCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC QPXAttrCurrentLimit_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);



static ViStatus _VI_FUNC QPXAttrOCPLimit_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 *value);

static ViStatus _VI_FUNC QPXAttrOCPLimit_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value);

static ViStatus _VI_FUNC QPXAttrIflock_ReadCallback (ViSession vi, ViSession io,
                                                     ViConstString channelName,
                                                     ViAttr attributeId,
                                                     ViBoolean *value);

static ViStatus _VI_FUNC QPXAttrIflock_WriteCallback (ViSession vi, ViSession io,
                                                      ViConstString channelName,
                                                      ViAttr attributeId,
                                                      ViBoolean value);

static ViStatus _VI_FUNC QPXAttrVoltageStepSize_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC QPXAttrVoltageStepSize_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC QPXAttrCurrentStepSize_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC QPXAttrCurrentStepSize_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);



static ViStatus _VI_FUNC QPXAttrLimitStatusRegister1_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister1_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister1_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC QPXAttrStandardEventEnableRegister_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrStandardEventEnableRegister_WriteCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViInt32 value);

static ViStatus _VI_FUNC QPXAttrIstLocalMessage_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrStandardEventStatusRegister_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrOperationCompleteStatus_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrOperationCompleteStatus_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 value);

static ViStatus _VI_FUNC QPXAttrParallelPoleEnableRegister_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrParallelPoleEnableRegister_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC QPXAttrServiceRequestEnableRegister_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrServiceRequestEnableRegister_WriteCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 value);

static ViStatus _VI_FUNC QPXAttrStatusByteRegister_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrAddress_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrVerifyVoltage_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean *value);

static ViStatus _VI_FUNC QPXAttrVerifyVoltage_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViBoolean value);

static ViStatus _VI_FUNC QPXAttrOutputEnabledAll_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);


static ViStatus _VI_FUNC QPXAttrLimitStatusRegister2_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister2_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister2_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);


static ViStatus _VI_FUNC QPXAttrDamping_WriteCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViBoolean value);



static ViStatus _VI_FUNC QPXAttrRemoteSenseMode_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViBoolean value);

static ViStatus _VI_FUNC QPXAttrRangeSetting_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrRangeSetting_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value);

static ViStatus _VI_FUNC QPXAttrRatio_ReadCallback (ViSession vi, ViSession io,
                                                    ViConstString channelName,
                                                    ViAttr attributeId,
                                                    ViReal64 *value);

static ViStatus _VI_FUNC QPXAttrRatio_WriteCallback (ViSession vi, ViSession io,
                                                     ViConstString channelName,
                                                     ViAttr attributeId,
                                                     ViReal64 value);

static ViStatus _VI_FUNC QPXAttrTrackingMode_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value);

static ViStatus _VI_FUNC QPXAttrTrackingMode_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value);

static ViStatus _VI_FUNC QPXAttrVoltageLevel_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value);

static ViStatus _VI_FUNC QPXAttrVoltageLevel_RangeTableCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 IviRangeTablePtr *rangeTablePtr);




/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: QPX_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           QPX_InitWithOptions function.   
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
	ViStatus    error = VI_SUCCESS;

	if (newVi == VI_NULL)
	{
		Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
		                  VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
		checkErr( IVI_ERROR_INVALID_PARAMETER);
	}

	checkErr( QPX_InitWithOptions (resourceName, IDQuery, resetDevice, 
	                                "", newVi));

	Error:  
	return error;
}

/*****************************************************************************
 * Function: QPX_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                            ViBoolean resetDevice, ViString optionString, 
                                            ViSession *newVi)
{   
	ViStatus    error = VI_SUCCESS;
	ViSession   vi = VI_NULL;
	ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
	ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
	ViBoolean   isLogicalName;

	if (newVi == VI_NULL)
	{
		Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
		                  VI_ERROR_PARAMETER5, "Null address for Instrument Handle");
		checkErr( IVI_ERROR_INVALID_PARAMETER);
	}

	*newVi = VI_NULL;

	checkErr( Ivi_GetInfoFromResourceName (resourceName, optionString, newResourceName,
	                                   newOptionString, &isLogicalName));

	/* create a new interchangeable driver */
	checkErr( Ivi_SpecificDriverNew ("QPX", newOptionString, &vi));  

	/* init the driver */
	checkErr( QPX_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
	if (isLogicalName)
		checkErr( Ivi_ApplyDefaultSetup (vi));
	*newVi = vi;

	Error:
	if (error < VI_SUCCESS) 
		Ivi_Dispose (vi);

	return error;
}

/*****************************************************************************
 * Function: QPX_IviInit                                                       
 * Purpose:  This function is called by QPX_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_IviInit (ViRsrc resourceName, ViBoolean IDQuery, ViBoolean reset, ViSession vi)
{
	ViStatus    error = VI_SUCCESS;
	ViSession   io = VI_NULL;
	ViChar rdBuffer[BUFFER_SIZE];
	
	ViInt32		trackingMode; 
	
	checkErr( Ivi_BuildChannelTable (vi, CHANNEL_LIST, VI_FALSE, VI_NULL));

	/* Add attributes */
	checkErr( QPX_InitAttributes (vi));

	if (!Ivi_Simulating(vi))
	{
		ViSession   rmSession = VI_NULL;

		/* Open instrument session */
		checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0, &rmSession)); 
		viCheckErr( viOpen (rmSession, resourceName, VI_LOAD_CONFIG, VI_NULL, &io));
		/* io session owned by driver now */																																	
		checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

		/* Configure VISA Formatted I/O */
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
		
		//!!	Added to overcome time out issues when communication with instrument on port 9221
		viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR_EN, VI_TRUE));
		//viCheckErr( viSetAttribute (io, VI_ATTR_SUPPRESS_END_EN, VI_FALSE));
		
		viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
		viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		//viCheckErr( viGetAttribute (io, VI_ATTR_ASRL_BAUD, &BaudRate));
		//viCheckErr( viSetAttribute (io, VI_ATTR_ASRL_BAUD, 19200));
		//viCheckErr( viGetAttribute (io, VI_ATTR_ASRL_BAUD, &BaudRate));
	}

	/*- Reset instrument ----------------------------------------------------*/
	if (reset) 
		checkErr( QPX_reset (vi));
	else  /*- Send Default Instrument Setup ---------------------------------*/
		checkErr( QPX_DefaultInstrSetup (vi));

	/*- Identification Query ------------------------------------------------*/

	if (IDQuery)                               
	{
		#define MUST_HAVE_IN_RESPONSE_STRING_1		 "THURLBY THANDAR"
		#define MUST_HAVE_IN_RESPONSE_STRING_2		 "QPX"
	

		checkErr( Ivi_GetAttributeViString (vi, VI_NULL, QPX_ATTR_ID_QUERY_RESPONSE, 0, BUFFER_SIZE, rdBuffer));

		if (strstr (rdBuffer, MUST_HAVE_IN_RESPONSE_STRING_1) == NULL || strstr (rdBuffer, MUST_HAVE_IN_RESPONSE_STRING_2) == NULL )
		{    
				viCheckErr( VI_ERROR_FAIL_ID_QUERY);
		}
		
		//configure the model				
		
		if (strstr (rdBuffer, "QPX1200") != NULL)
			PSUModel = QPX1200;
		else if (strstr (rdBuffer, "QPX600") != NULL)
			PSUModel = QPX600;
	
		else
			PSUModel = QPX1200;					
		

		
		
		checkErr( Ivi_GetAttributeViString (vi, VI_NULL, QPX_ATTR_INSTRUMENT_MODEL, 0, BUFFER_SIZE, rdBuffer));				
	}


	if(PSUModel == QPX1200)					 // Force Tracking Mode to 1 - Only valid value for single channel supplies
	error = Ivi_SetAttributeViInt32 (vi,NULL, QPX_ATTR_TRACKING_MODE,
                                      IVI_VAL_DIRECT_USER_CALL, 1);
	
	//setup range tables with model specific information
	
	
	checkErr( QPX_InitRangeTables (vi));
	
	checkErr( QPX_CheckStatus (vi));	  

	Error:
	if (error < VI_SUCCESS)
	{
		if (!Ivi_Simulating (vi) && io)
		  viClose (io);
	}				   
	return error;	   
}

/*****************************************************************************
 * Function: QPX_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_close (ViSession vi)
{
  ViStatus    error = VI_SUCCESS;
  
  checkErr( Ivi_LockSession (vi, VI_NULL));
  
  checkErr( QPX_IviClose (vi));

	Error:    
  Ivi_UnlockSession (vi, VI_NULL);
  Ivi_Dispose (vi);  

  return error;
}
/*****************************************************************************
 * Function: QPX_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           QPX_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_IviClose (ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	ViSession io = VI_NULL;

	/* Do not lock here.  The caller manages the lock. */

	checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, &io));

	Error:
	Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL);
	if(io)                                                      
	{
		viClose (io);
	}
	return error;   
}

/*****************************************************************************
 * Function: QPX_WaitForOperationComplete  
 * Purpose:  Wait for Operation Complete true. As all commands are completely
 *					 executed before the next is started this command takes no
 *					 additional action.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_WaitForOperationComplete (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "*WAI\n"));
	}

	checkErr( QPX_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_ClearStatus  
 * Purpose:  Clears the Standard Event Status Register, Query Error Register
 *					 and Execution Error Register. This indirectly clears the Status
 *					 Byte Register.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ClearStatus (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "*CLS\n"));
	}

	checkErr( QPX_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_reset (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		//this is required in the current version of the firmware (0.27), WH said OK to leave in [4/9/08]
		viCheckErr( viPrintf (io, "*RMT\n"));
		viCheckErr( viPrintf (io, "*RST\n"));
	}

	Delay(0.1);
	checkErr( QPX_DefaultInstrSetup (vi));                                
	checkErr( QPX_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_SaveSetup
 * Purpose: Saves the current set-up of channel requested to the set-up
 *					store specified which can be 0-9.
 *					If the instrument is operating in linked mode then the entire
 *					instrument set-up will be stored in the linked mode set-up store
 *					specified. The channel specification is ignored. This has no
 *					affect on the individual channel set-up stores available when
 *					not in linked mode 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_SaveSetup (ViSession vi,
                                  ViChar channelName[], ViInt32 store)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "SAV%s %d\n", channelName, store));
	}

	checkErr( QPX_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_RecallSetup
 * Purpose: Recall a set up for the channel selected from the set-up store
 *					specified which can be 0-9.
 *					If the instrument is operating in linked mode then the entire
 *					instrument set-up will be recalled from the linked mode set-up
 *					store specified. The channel specification is ignored. 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_RecallSetup (ViSession vi,
                                    ViChar channelName[], ViInt32 store)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "RCL%s %d\n", channelName, store));
	}

	checkErr( QPX_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_Local
 * Purpose: Switches the instrument to local mode. 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_Local (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "LOCAL\n"));
	}

	//can't do these check as this would force the instrument back into remote
	
	//checkErr( QPX_DefaultInstrSetup (vi));                                

	//checkErr( QPX_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (testResult == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
	if (testMessage == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "*TST?\n"));

		viCheckErr( viScanf (io, "%hd", testResult));

		sprintf ( testMessage, "Self test code: %i", *testResult);
	}
	else
	{
		/* Simulate Self Test */
		*testResult = 0;
		strcpy (testMessage, "No error.");
	}

	checkErr( QPX_CheckStatus (vi));

	Error:
		Ivi_UnlockSession(vi, VI_NULL);
		return error;
}

/*****************************************************************************
 * Function: QPX_query_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (errCode == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code");
	if (errMessage == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		//viCheckErr( viPrintf(io, "QER?\n"));
		viCheckErr( viQueryf (io, "QER?\n", "%ld", &errCode));

		//viCheckErr( viScanf (io, "%d", errCode));

		sprintf( errMessage, "Error code: %d", *errCode);
	}
	else
	{
		/* Simulate Error Query */
		*errCode = 0;
		strcpy (errMessage, "No error.");
	}

	Error:
		Ivi_UnlockSession(vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_execution_error_query                                                     
 * Purpose:  This function queries the instrument execution error queue and    
 *           returns the result.                                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_execution_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (errCode == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code");
	if (errMessage == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf(io, "EER?\n"));

		viCheckErr( viScanf (io, "%d", errCode));

		sprintf( errMessage, "Error code: %d", *errCode);
	}
	else
	{
		/* Simulate Error Query */
		*errCode = 0;
		strcpy (errMessage, "No error.");
	}

	Error:
		Ivi_UnlockSession(vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
	ViStatus    error = VI_SUCCESS;

	static      IviStringValueTable errorTable = 
	{
		/*=PTP KCS================================================================*
		    Insert instrument driver specific error codes here.  Example:                        

		    {QPX_ERROR_TOO_MANY_SAMPLES,   "Sample Count cannot exceed 512."},   
		 *=============================================================END=PTP KCS*/
		IVIDCPWR_ERROR_CODES_AND_MSGS,
	  {VI_NULL,                               VI_NULL}
	};

	if (vi)
		Ivi_LockSession(vi, VI_NULL);

	/* all VISA and IVI error codes are handled as well as codes in the table */
	if (errorMessage == VI_NULL)
		viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

	checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable));

	Error:
	if (vi)
		Ivi_UnlockSession(vi, VI_NULL);
		
	return error;
}

/*****************************************************************************
 * Function: QPX_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
  ViStatus    error = VI_SUCCESS;
  
  checkErr( Ivi_LockSession (vi, VI_NULL));

  if (driverRev == VI_NULL)
    viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
  if (instrRev == VI_NULL)
    viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

  checkErr( Ivi_GetAttributeViString (vi, VI_NULL, QPX_ATTR_SPECIFIC_DRIVER_REVISION, 
                                      0, 256, driverRev));
  checkErr( Ivi_GetAttributeViString (vi, "", QPX_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                      0, 256, instrRev));
  checkErr( QPX_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: QPX_ConfigureOutputEnabled                                   
 * Purpose:  This function controls whether or not the signal the power supply
 *           generates is present at the output terminal.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureOutputEnabled (ViSession vi,
                                                   ViConstString channelName,
                                                   ViBoolean enabled)
{
    ViStatus error = VI_SUCCESS;

	
	checkErr( Ivi_LockSession (vi, VI_NULL));
	

	

	error = Ivi_SetAttributeViBoolean (vi, channelName, QPX_ATTR_OUTPUT_ENABLED,
                                      IVI_VAL_DIRECT_USER_CALL, enabled);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;
}



/*****************************************************************************
 * Function: QPX_ConfigureCurrentLimit                                   
 * Purpose:  This function configures the behavior and level of the
 *           current limit.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureCurrentLimit (ViSession vi,
                                              ViChar channelName[],
                                              ViInt32 behavior,
                                              ViReal64 limit)
{
	ViStatus error = VI_SUCCESS;
	
	
	Ivi_LockSession(vi, VI_NULL);
	
	if (behavior == QPX_VAL_CURRENT_REGULATE || behavior == QPX_VAL_CURRENT_TRIP)
	{
		error = Ivi_SetAttributeViReal64 (vi, channelName, QPX_ATTR_CURRENT_LIMIT, 0, limit);		
	}
	Error:    
	
	Ivi_UnlockSession(vi, VI_NULL);

	return  error ;
	
}

/*****************************************************************************
 * Function: QPX_ConfigureOVP                                   
 * Purpose:  This function enables or disables over-voltage protection and
 *           configures the over-voltage limit.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureOVP (ViSession vi,ViConstString channelName,
										 ViBoolean enabled,
                                         ViReal64 limit)
{
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	//If you are enabling over-voltage protection, set the limit first
	//to avoid a premature over-voltage condition.

/*	
	if (enabled)
	{
		viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
		                                   QPX_ATTR_OVP_LIMIT,
		                                   0, limit), 4, "OVP Limit");
	}
	
*/	


	if(PSUModel == QPX1200)
		viCheckErr( Ivi_CheckNumericRange(limit, 2, 65, IVI_ERROR_INVALID_VALUE));  
	



	viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
                                   QPX_ATTR_OVP_LIMIT,
                                   0, limit), 4, "OVP Limit");		
		
		
/*	
	if(enabled){
		
			if( (PSUModel == QPX1200) && (limit > 65) )  {
					viCheckErrElab( IVI_ERROR_INVALID_VALUE,
               			"OVP maximum for QPX1200 is 65V");
				
					error = Ivi_SetAttributeViReal64 (vi, channelName, QPX_ATTR_OVP_LIMIT, 0, limit);					
			}
				
			else{
				viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
		                                   QPX_ATTR_OVP_LIMIT,
		                                   0, limit), 4, "OVP Limit");				
			}
		
	}
*/	
	
	viCheckParm( Ivi_SetAttributeViBoolean (vi, channelName, 
	                                    QPX_ATTR_OVP_ENABLED,
	                                    0, enabled), 3, "OVP Enabled");
    
	checkErr( QPX_CheckStatus (vi));

Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: QPX_ConfigureOCP                                   
 * Purpose:  This function enables or disables over-current protection and
 *           configures the over-current limit.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureOCP (ViSession vi,
                                         ViConstString channelName,
                                         ViReal64 limit)
{
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
	                                 QPX_ATTR_OCP_LIMIT,
	                                 0, limit), 4, "OCP Limit");
	checkErr( QPX_CheckStatus (vi));

Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	return error;
}


/*****************************************************************************
 * Function: QPX_ConfigureRatio                                   
 * Purpose:  This function sets the ratio between the voltage level of output 1 
 			 and output 2
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureRatio (ViSession vi,
										 ViConstString channelName,
                                         ViReal64 ratio)
{
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	
	if(PSUModel == QPX600)
		error = Ivi_SetAttributeViReal64 (vi, NULL, QPX_ATTR_RATIO, 1, ratio);      
	else
		viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION, 
                        "QPX1200 does not support RATIO function.");
	
	checkErr( QPX_CheckStatus (vi));

Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	return error;
}


/*****************************************************************************
 * Function: QPX_ConfigureTrackingMode                                   
 * Purpose:  This function sets the ratio between the voltage level of output 1 
 			 and output 2
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureTrackingMode (ViSession vi,
										 ViConstString channelName,
                                         ViInt32 trackingMode)
{
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if(PSUModel == QPX600)
	{	
		error = Ivi_SetAttributeViInt32 (vi, NULL, QPX_ATTR_TRACKING_MODE, 0, trackingMode);
	}
	else
	{
		  viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION, 
                        "QPX1200 is a single channel instrument.");
	}	  
	
	
	
//	checkErr( QPX_CheckStatus (vi));

Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	return error;
}







/*****************************************************************************
 * Function: QPX_ConfigureVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ConfigureVoltageLevel (ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 level)
{
    
	ViInt32	range;
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	checkErr(Ivi_GetAttributeViInt32 (vi, channelName, QPX_ATTR_RANGE_SETTING,
                                       0, &range));
	

	
	if(PSUModel == QPX600)
		{	
			switch(range)
			{
		
			case 1:							 // 60V range
				
				if(level > 60)
					viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               			"Change range for Voltage Level greater than 60V");
				
				error = Ivi_SetAttributeViReal64 (vi, channelName, QPX_ATTR_VOLTAGE_LEVEL, 1, level);			 
				break;
			case 2:							 // 80V range
				error = Ivi_SetAttributeViReal64 (vi, channelName, QPX_ATTR_VOLTAGE_LEVEL, 0, level);			   
				break;
		
			default:
				break;
			}	
		}
		else
		{	
	
			viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName,
                                               QPX_ATTR_VOLTAGE_LEVEL, 0,
    		                                       level), 3, "Level");
			
		}
		
	
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;
}

/*****************************************************************************
 * Function: QPX_QueryMaxCurrentLimit                  
 * Purpose:  This function returns the maximum programmable current limit
 *           that the power supply accepts for a particular voltage level on
 *           a channel for the output range to which the power supply is
 *           currently configured.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64* maxCurrentLimit)
{
  ViStatus error = VI_SUCCESS;
  ViReal64 value;
	ViChar rdBuffer[BUFFER_SIZE];
  
  checkErr( Ivi_LockSession(vi, VI_NULL));

  if (maxCurrentLimit == VI_NULL)
          viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxCurrentLimit.");
  
  if (Ivi_RangeChecking(vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),  2, "Channel Name");
	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
	  switch (PSUModel)
		{
			case QPX1200:
			default:	
				value = 50.0;
				break;
			case QPX600:
				value = 50.0;				 
				break;

		}
  }
	
  checkErr( QPX_CheckStatus (vi));

Error:    
  if (maxCurrentLimit) *maxCurrentLimit = value;
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: QPX_QueryMaxVoltageLevel                  
 * Purpose:  This function returns the maximum programmable voltage level
 *           that the power supply accepts for a particular current limit on
 *           a channel for the output range to which the power supply is
 *           currently configured.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64* maxVoltageLevel)
{															 
  ViStatus error = VI_SUCCESS;
  ViReal64 value;
	ViChar rdBuffer[BUFFER_SIZE];
  
  checkErr( Ivi_LockSession(vi, VI_NULL));

  if (maxVoltageLevel == VI_NULL)
          viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxVoltageLevel.");
  
  if (Ivi_RangeChecking(vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
	  switch (PSUModel)
		{
			case QPX1200:
			default:	
				value = 60.0;
				break;
			case QPX600:
				value = 80.0;
				break;

		}
	}
	
  checkErr( QPX_CheckStatus (vi));

Error:    
  if (maxVoltageLevel) *maxVoltageLevel = value;
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: QPX_ResetOutputProtection                                   
 * Purpose:  This function resets the power supply's output protection.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ResetOutputProtection (ViSession vi, ViConstString channelName)
{
  ViStatus error = VI_SUCCESS;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
  
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  
    viCheckErr( viPrintf (io, "TRIPRST\n"));
  }
  checkErr( QPX_CheckStatus (vi));
  
Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
          
}

/*****************************************************************************
 * Function: QPX_IncrementVoltageLevel
 * Purpose: Increments the output voltage by the step size set for the
 *					output channel specified.
 *
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_IncrementVoltageLevel (ViSession vi,
                                              ViChar channelName[])
{
  ViStatus error = VI_SUCCESS;
  ViAttr   attrId;
  ViInt32		trackingMode;
  ViInt32		retCnt;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (Ivi_RangeChecking (vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
  }

  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
	
	
	char Ret[15];
	
	viCheckErr( viPrintf (io, "CONFIG?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%d",&trackingMode);   
  
    if( (trackingMode != 2) && ( strcmp(channelName , "2") == 0 ) )
	 viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must be in Dual Output Unlinked mode to set V2");	
	
      
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    if (VerifyVoltage)
      viCheckErr( viPrintf (io, "INCV%sV\n", channelName));
    else
      viCheckErr( viPrintf (io, "INCV%s\n", channelName));
  }

  checkErr( QPX_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: QPX_DecrementVoltageLevel
 * Purpose: Decrements the output voltage by the step size set for the
 *					output channel specified. 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_DecrementVoltageLevel (ViSession vi,
                                              ViChar channelName[])
{
  ViStatus error = VI_SUCCESS;
  ViAttr   attrId;
  
  
   ViInt32		trackingMode;
   ViInt32		retCnt;
  
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (Ivi_RangeChecking (vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
  	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
  }

  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
	
	char Ret[15];
	
	viCheckErr( viPrintf (io, "CONFIG?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%d",&trackingMode); 
	
    if( (trackingMode != 2) && ( strcmp(channelName , "2") == 0 ) )
	 viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must be in Dual Output Unlinked mode to set V2");	
	
	
	
      
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    if (VerifyVoltage)
    	viCheckErr( viPrintf (io, "DECV%sV\n", channelName));
    else
    	viCheckErr( viPrintf (io, "DECV%s\n", channelName));
  }

  checkErr( QPX_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: QPX_IncrementCurrentLevel
 * Purpose:  Increments the current level limit by the step size set for the
 *						output channel specified.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_IncrementCurrentLevel (ViSession vi,
                                              ViChar channelName[])
{
  ViStatus error = VI_SUCCESS;
  ViAttr   attrId;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (Ivi_RangeChecking (vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),2, "Channel Name");
  	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
  }

  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
      
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    viCheckErr( viPrintf (io, "INCI%s\n", channelName));
  }

  checkErr( QPX_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: QPX_DecrementCurrentLevel
 * Purpose: Decrements the current level limit by the step size set for the
 *					output channel specified.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_DecrementCurrentLevel (ViSession vi,
                                              ViChar channelName[])
{
  ViStatus error = VI_SUCCESS;
  ViAttr   attrId;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (Ivi_RangeChecking (vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
  }

  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
      
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    viCheckErr( viPrintf (io, "DECI%s\n", channelName));
  }

  checkErr( QPX_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: QPX_Measure                                   
 * Purpose:  This function measures either the current or voltage that the
 *           power supply is generating.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static IviRangeTableEntry measurementTypeRangeTableEntries[] =
{
	{QPX_VAL_MEASURE_CURRENT, 0, 0, "I", QPX_ATTR_CURRENT_LIMIT},
	{QPX_VAL_MEASURE_VOLTAGE, 0, 0, "V", QPX_ATTR_VOLTAGE_LEVEL},
	{IVI_RANGE_TABLE_LAST_ENTRY}
};
static IviRangeTable measurementTypeRangeTable =
{
	IVI_VAL_DISCRETE,
  VI_FALSE,
  VI_FALSE,
  VI_NULL,
  measurementTypeRangeTableEntries,
};

ViStatus _VI_FUNC QPX_Measure (ViSession vi,
                                   ViConstString channelName,
                                   ViInt32 measurementType,
                                   ViReal64* measurement)
{
  ViStatus error = VI_SUCCESS;
  ViReal64 value = 0.0;
  ViAttr   attrId;
  ViString cmd;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (measurement == VI_NULL)
    viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for measurement.");

  if (Ivi_RangeChecking (vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),2, "Channel Name");
	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
  }
  Ivi_GetViInt32EntryFromValue (measurementType,
                                &measurementTypeRangeTable, 
                                VI_NULL, VI_NULL, VI_NULL,
                                VI_NULL, &cmd, &attrId);
  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
      
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    viCheckErr( viQueryf (io, "%s%sO?\n", "%lf", cmd, channelName, &value));
  }
  else
  {
    checkErr( Ivi_GetAttributeViReal64 (vi, channelName, attrId, 0, &value));
    value *= 0.95;
  }


  checkErr( QPX_CheckStatus (vi));

Error:    
  if (measurement) *measurement = value;
  	Ivi_UnlockSession(vi, VI_NULL);
  return error;
          
}

/*****************************************************************************
 * Function: QPX_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC QPX_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC QPX_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC QPX_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC QPX_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: QPX_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC QPX_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC QPX_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC QPX_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC QPX_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: QPX_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the QPX_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC QPX_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC QPX_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC QPX_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC QPX_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/*****************************************************************************
 * Function: QPX_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/*****************************************************************************
 * Function: QPX_LockSession and QPX_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC QPX_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/*****************************************************************************
 * Function: QPX_GetErrorInfo and QPX_ClearErrorInfo Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC QPX_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}
ViStatus _VI_FUNC QPX_GetError (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;                                  
}                                                                                                           
ViStatus _VI_FUNC QPX_ClearError (ViSession vi)                                                        
{                                                                                                           
     ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_ClearErrorInfo (vi); 
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;                                                                             
}


/*****************************************************************************
 * Function: WriteInstrData and ReadInstrData Functions                      
 * Purpose:  These functions enable the instrument driver user to  
 *           write and read commands directly to and from the instrument.            
 *                                                                           
 *           Note:  These functions bypass the IVI attribute state caching.  
 *                  WriteInstrData invalidates the cached values for all 
 *                  attributes.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC QPX_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 


/*------------[NEW]----------------------------------------------------------*/
/*****************************************************************************
 * Function: QPX_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
*****************************************************************************/
ViStatus _VI_FUNC QPX_InvalidateAllAttributes (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_InvalidateAllAttributes(vi);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;    
}

/*****************************************************************************
 * Function: QPX_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( QPX_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}
/**************************************************************************** 
 *  Function: QPX_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC QPX_Disable (ViSession vi)
{
     ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

					 
	error = Ivi_SetAttributeViBoolean (vi, VI_NULL, QPX_ATTR_OUTPUT_ENABLED_ALL,
                                   IVI_VAL_DIRECT_USER_CALL, VI_FALSE);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;

}

/**************************************************************************** 
 *  Function: QPX_GetNextInterchangeWarning,
 *            QPX_ResetInterchangeCheck, and
 *            QPX_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC QPX_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;     
}

/*****************************************************************************
 * Function: QPX_ResetInterchangeCheck                                       
 * Purpose:                                       
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ResetInterchangeCheck (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_ResetInterchangeCheck (vi);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;    
}

/*****************************************************************************
 * Function: QPX_ClearInterchangeWarnings                                       
 * Purpose:                                       
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_ClearInterchangeWarnings (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_ClearInterchangeWarnings (vi);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;    
}




/*****************************************************************************
 * Function: QPX_ConfigureOutputRange                                       
 * Purpose:  Configures the power supply's output range on a channel.                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/


ViStatus _VI_FUNC QPX_ConfigureOutputRange (ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 rangeType,
                                                 ViReal64 range)
{
  ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));


	if(PSUModel == QPX600)
	{	
	
		if (rangeType == QPX_VAL_RANGE_CURRENT || rangeType == QPX_VAL_RANGE_VOLTAGE)
		{	
			error = Ivi_SetAttributeViInt32 (vi, channelName, QPX_ATTR_RANGE_SETTING,
	        	                           IVI_VAL_DIRECT_USER_CALL, range);
		}
	}
	else
	{
		viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION, 
                        "QPX1200 does not support ranges.");
	}	
	
	
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;

            
}



	

/*****************************************************************************
 * Function: QPX_QueryOutputState                                       
 * Purpose:  This function returns whether the power supply is in a
 *					 particular output state.                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC QPX_QueryOutputState (ViSession vi,
                                             ViConstString channelName,
                                             ViInt32 outputState,
                                             ViBoolean* inState)
{
	
	
   ViStatus error = VI_SUCCESS;
   ViBoolean value = VI_FALSE;  
   
   ViInt8	stat_reg = 0;
   


	checkErr( Ivi_LockSession (vi, VI_NULL));
	

    viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
	
	//viCheckErr( viQueryf (io, "OUTP:%s:COND?", "%hd", cmd, &value));      
	
	
	if (!Ivi_Simulating (vi))
    {
		ViSession io = Ivi_IOSession(vi);  
		viCheckErr( viQueryf (io, "LSR%s?\n", "%hd" ,channelName, &stat_reg)); 
	
		// read it again to get current state
		
		if( (outputState == QPX_VAL_OUTPUT_CONSTANT_VOLTAGE) || (outputState == QPX_VAL_OUTPUT_CONSTANT_CURRENT)){
			Delay(0.2);
			viCheckErr( viQueryf (io, "LSR%s?\n", "%hd" ,channelName, &stat_reg));  
		}		
		
		
//		if( (outputState == TQLP_VAL_OUTPUT_CONSTANT_VOLTAGE) || (outputState == TQLP_VAL_OUTPUT_CONSTANT_CURRENT)){
//			Delay(0.2);
//			viCheckErr( viQueryf (io, "LSR%s?\n", "%hd" ,channelName, &stat_reg));  
//		}
	}
	
	
	// mask off top 4 bits
	stat_reg = stat_reg & 0x0F;
	
	
	switch(outputState)
	{
		case QPX_VAL_OUTPUT_CONSTANT_VOLTAGE:	
			 stat_reg &= 0x01;
			 break;
		case QPX_VAL_OUTPUT_CONSTANT_CURRENT:	
			 stat_reg &= 0x02;
			 break;		

		case QPX_VAL_OUTPUT_UNREGULATED:	
			 stat_reg &= 0x08;
			 break;
			 
			 
		case QPX_VAL_OUTPUT_OVER_VOLTAGE:	
			 stat_reg &= 0x08;
			 break;
		case QPX_VAL_OUTPUT_OVER_CURRENT:	
			 stat_reg &= 0x10;
			 break;	
		default: 
		// Invalid query.
			viCheckErr( IVI_ERROR_INVALID_PARAMETER );		
			break; 			 
	}	
	
	if(stat_reg != 0)
		*inState = VI_TRUE;
	else
		*inState = VI_FALSE;
	
	Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;	
	
	
/*   ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	
	switch (outputState)
	{
		case QPX_VAL_OUTPUT_CONSTANT_VOLTAGE:
		case QPX_VAL_OUTPUT_CONSTANT_CURRENT:
		case QPX_VAL_OUTPUT_OVER_VOLTAGE:
		case QPX_VAL_OUTPUT_OVER_CURRENT:    
		case QPX_VAL_OUTPUT_UNREGULATED:     
			//this power supply does not support any of the modes so always return false.
			inState = VI_FALSE;
			break;	
		default:	
			//invalid so false
			inState = VI_FALSE;
			break;	
	}


	Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;
*/
	
/*	
	ViStatus error = VI_SUCCESS;
    ViBoolean value = VI_FALSE;
    ViString cmd;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    if (inState == VI_NULL)
            viCheckParm (IVI_ERROR_INVALID_PARAMETER, 2, "Null value for inState.");
    
    viCheckParm( Ivi_GetViInt32EntryFromValue (outputState,
                                               &outputStateRangeTable,
                                               VI_NULL, VI_NULL, VI_NULL, 
                                               VI_NULL, &cmd, VI_NULL),
                                               3, "Output State");
    if (Ivi_RangeChecking(vi))
        {
        viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),
                                            2, "Channel Name");
        }
    
    if (!Ivi_Simulating (vi))
        {
        ViSession io = Ivi_IOSession(vi);
        
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viQueryf (io, "OUTP:%s:COND?", "%hd", cmd, &value));
        }

    checkErr( QPX_CheckStatus (vi));

Error:    
    if (inState) *inState = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
*/            
}

/**************************************************************************** 
 *  Function: QPX_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC QPX_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[])
{
    ViStatus         error = VI_SUCCESS;
    ViConstString    channelName;
    ViInt32          actualSize = 0;
    ViInt32			channelCount;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    /* Check parameters */
    if (bufferSize != 0)
        {
        if (name == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name");
            }
        }
        
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, IVI_ATTR_CHANNEL_COUNT, 0, &channelCount));
    if ( (index<1) || (index>channelCount) )
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Channel Index");
    
    /* Get channel name */
    viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    checkErr( Ivi_GetUserChannelName (vi, channelName, &channelName));
    
    actualSize = strlen(channelName) + 1;
    
    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(name, channelName, actualSize-1);
        name[actualSize-1] = '\0';
        }

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}
/*---------------------------------------------------------------------------*/



/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/
/*****************************************************************************
 *  Function: QPX_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option.  The driver setup string
 *            is part of the option string the user passes to the
 *            QPX_InitWithOptions function.  Specifically it is the part
 *            that follows "DriverSetup=".  This function assumes the 
 *            DriverSetup string has the following format:
 *              "OptionTag1 : OptionValue1; OptionTag2 : OptionValue2"
 *            Leading and trailing white space is removed from the option
 *            value and both the option tag and option value may contain
 *            white space.  The option tag is case sensitive.
 *
 *            Example:
 *
 *              "DriverSetup= Model : InstrX ; Special Option:Feature Y "
 *
 *              optionTag "Model"          returns optionValue "InstrX"
 *
 *              optionTag "Special Option" returns optionValue "Feature Y"
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus QPX_GetDriverSetupOption (ViSession vi,
                                               ViConstString optionTag,
                                               ViInt32 optionValueSize,
                                               ViChar optionValue[])
{
  ViStatus error = VI_SUCCESS;
  ViChar   setup[BUFFER_SIZE];
  ViChar*  searchOrigin;
  ViChar*  start;
  ViChar*  end;
  ViUInt32 n;

  checkErr( Ivi_LockSession (vi, VI_NULL));

  if ((optionValue != VI_NULL) && (optionValueSize > 0))
    optionValue[0] = 0;

  if ((optionTag != VI_NULL) && (optionValue != VI_NULL) && (optionValueSize > 1))
  {
	  checkErr( Ivi_GetAttributeViString (vi, "", IVI_ATTR_DRIVER_SETUP,
	                                      0, BUFFER_SIZE, setup));
	  searchOrigin = setup;
	  while (searchOrigin != VI_NULL)
    {
		  searchOrigin = strstr(searchOrigin, optionTag);
		  if (searchOrigin)
      {
	      searchOrigin += strlen(optionTag);
	      start = searchOrigin;
	      while (isspace(*start)) start++;
	      if (*start == ':')
	      {
		      start++;
		      while (isspace(*start)) start++;
		      if (*start)
          {
	          end = strchr (start, ';');
	          if (start != end)
            {
	            end = (end) ? end-1 : &setup[strlen(setup)-1];
	            while (isspace(*end)) end--;
	            n = end - start + 1;
	            if (n > optionValueSize - 1)
	                n = optionValueSize - 1;
	            strncpy(optionValue, start, n);
	            optionValue[n] = 0;
	            break;
            }
          }
	      }
      }
    }
  }
      
Error:
  Ivi_UnlockSession (vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: QPX_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTR_SPECIFIC.  The user can set the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus QPX_CheckStatus (ViSession vi)
{
  ViStatus    error = VI_SUCCESS;

  if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
  {
	  checkErr( QPX_CheckStatusCallback (vi, Ivi_IOSession(vi)));
	  checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
  }
      
Error:
    return error;
}

/*****************************************************************************
 * Function: QPX_WaitForOPC                                                  
 * Purpose:  This function waits for the instrument to complete the      
 *           execution of all pending operations.  This function is a        
 *           wrapper for the WaitForOPCCallback.  It can be called from 
 *           other instrument driver functions. 
 *
 *           The maxTime parameter specifies the maximum time to wait for
 *           operation complete in milliseconds.
 *
 *           Note:  Call this function only when the session is locked.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus QPX_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
  ViStatus    error = VI_SUCCESS;

  if (!Ivi_Simulating(vi))
  {
	  ViInt32  oldOPCTimeout; 
  
	  checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, QPX_ATTR_OPC_TIMEOUT, 
	                                     0, &oldOPCTimeout));
	  Ivi_SetAttributeViInt32 (vi, VI_NULL, QPX_ATTR_OPC_TIMEOUT,        
	                           0, maxTime);

	  error = QPX_WaitForOPCCallback (vi, Ivi_IOSession(vi));

	  Ivi_SetAttributeViInt32 (vi, VI_NULL, QPX_ATTR_OPC_TIMEOUT, 
	                           0, oldOPCTimeout);
	  viCheckErr( error);
  }
Error:
  return error;
}

/*****************************************************************************
 * Function: QPX_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           QPX_reset function calls this function.  The 
 *           QPX_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus QPX_DefaultInstrSetup (ViSession vi)
{
  ViStatus    error = VI_SUCCESS;
      

  /* Invalidate all attributes */
  checkErr( Ivi_InvalidateAllAttributes (vi));
  
  if (!Ivi_Simulating(vi))
  {
	  ViSession   io = Ivi_IOSession(vi); /* call only when locked */

	  checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		/*=PTP KCS:===============================================================*
		The default setup does the following: 

		*CLS     clears the event/status registers                                      
		*ESE 1   sets the standard event status enable register to recognize  
		        operation complete.                                          
		*SRE 32  sets the service request register to enable a service        
		        request on operation complete                                

		These settings are required for the default implementation of the      
		WaitForOPCCallback to work correctly.                                 

		*===========================================================END=PTP KCS=*/
		Delay(0.100);
		viCheckErr( viPrintf (io, "*CLS;*ESE 1;*SRE 32\n"));

  }
Error:
    return error;
}

/*****************************************************************************
 * Function: ReadToFile and WriteFromFile Functions                          
 * Purpose:  Functions for instrument driver developers to read/write        
 *           instrument data to/from a file.                                 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus QPX_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus QPX_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: QPX_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           QPX_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus _VI_FUNC QPX_CheckStatusCallback (ViSession vi, ViSession io)
{
  ViStatus    error = VI_SUCCESS;
  ViInt16     esr = 0; 
  //char ErrMsg[256];
  //ViInt32 Ret;

  
	/* Query instrument status */
  viCheckErr( viWrite (io, "*ESR?\n", 6, VI_NULL));

  viCheckErr( viScanf (io, "%hd", &esr));

  if (
  		/*(esr & IEEE_488_2_QUERY_ERROR_BIT) ||
  			This was removed after consultation with John Tothill 31/3/03
  		*/	
      (esr & IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT) ||
      (esr & IEEE_488_2_EXECUTION_ERROR_BIT) ||
      (esr & IEEE_488_2_COMMAND_ERROR_BIT))
  {     
		//QPX_query_error_query( vi, &Ret, ErrMsg);
	  viCheckErr( IVI_ERROR_INSTR_SPECIFIC);
  }
          
Error:
  return error;
}

/*****************************************************************************
 * Function: QPX_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           QPX_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus _VI_FUNC QPX_WaitForOPCCallback (ViSession vi, ViSession io)
{
  ViStatus    error = VI_SUCCESS;
  /*=PTP KCS:===============================================================*
     This function assumes that the instrument does not generate service requests 
     for other reasons and operates as follows example performs the following operations
     
     - Enables the service request event.
     - Sends a command that forces the instrument to generate a service 
       request.
     - Waits for the service request.
     - Cleans up after the service request.

   *============================================================END=PTP KCS*/
  ViInt32     opcTimeout;
  ViUInt16    statusByte;

  checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, QPX_ATTR_OPC_TIMEOUT, 
                                     0, &opcTimeout));

  viCheckErr( viEnableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL));

  viCheckErr( viPrintf (io, "*OPC"));

  // wait for SRQ 
  viCheckErr( viWaitOnEvent (io, VI_EVENT_SERVICE_REQ, opcTimeout, VI_NULL, VI_NULL));
  viCheckErr( viDisableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE));

  // clean up after SRQ 
  viCheckErr( viBufWrite (io, "*CLS", 4, VI_NULL));
  viCheckErr( viReadSTB (io, &statusByte));
  
Error:
  viDiscardEvents (io, VI_EVENT_SERVICE_REQ, VI_QUEUE);

  return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/

/*- QPX_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC QPXAttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      rdBuffer[BUFFER_SIZE];
  ViUInt32    retCnt;
  
  
  viCheckErr( viPrintf (io, "*IDN?\n"));
  viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE-1, &retCnt));
  rdBuffer[retCnt] = 0;

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
  
Error:
  return error;
}
    
/*- QPX_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC QPXAttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      driverRevision[256];
  
  
  sprintf (driverRevision, 
           "Driver: QPX %i.%i, Compiler: %s %3.2f, "
           "Components: IVIEngine %.2f, VISA-Spec %.2f",
           QPX_MAJOR_VERSION, QPX_MINOR_VERSION, 
           IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
           IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
           Ivi_ConvertVISAVer(VI_SPEC_VERSION));

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
  return error;
}


/*- QPX_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC QPXAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      instrRev[BUFFER_SIZE];
  
  viCheckErr( viPrintf (io, "*IDN?\n"));

	viCheckErr( viScanf (io, "%*[^,],%*[^,],%*[^,],%256[^\n]", instrRev));

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, instrRev));
  
Error:
  return error;
}

/*- QPX_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC QPXAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      rdBuffer[BUFFER_SIZE];
  ViChar      idQ[BUFFER_SIZE];
  
  checkErr( Ivi_GetAttributeViString (vi, "", QPX_ATTR_ID_QUERY_RESPONSE,
                                      0, BUFFER_SIZE, idQ));
  sscanf (idQ, "%256[^,]", rdBuffer);

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
  
Error:
  return error;
}

    

/*- QPX_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC QPXAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      rdBuffer[BUFFER_SIZE];
  ViChar      idQ[BUFFER_SIZE];
  
  checkErr( Ivi_GetAttributeViString (vi, "", QPX_ATTR_ID_QUERY_RESPONSE,
                                      0, BUFFER_SIZE, idQ));
  sscanf (idQ, "%*[^,],%256[^,]", rdBuffer);

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
  
Error:
  return error;
}
    

/*- QPX_ATTR_OUTPUT_ENABLED -*/

static ViStatus _VI_FUNC QPXAttrOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
  ViStatus error = VI_SUCCESS;
  
  	ViInt32		trackingMode;
    ViInt32		retCnt;
	char Ret[15];  
	
	int  channel;

 	viCheckErr( viPrintf (io, "CONFIG?\n"));
    viCheckErr( viRead (io, Ret, 14, &retCnt));
    sscanf(Ret,"%d",&trackingMode);   	
	
	channel = atoi(channelName);      
	
	if( (   (trackingMode == 3) || (trackingMode == 4) ) && (channel == 2 ) )
	 
		viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION, 
                        "Must enable Channel 1 or use QPX_ATTR_OUTPUT_ENABLED_ALL in pseudo series and parallel modes");
  
  
  viCheckErr( viPrintf (io, "OP%s %hd\n", channelName, value ? 1 : 0));
  
Error:
  return error;
}


/*- QPX_ATTR_VOLTAGE_LEVEL -*/
static IviRangeTableEntry attrVoltageLevelRangeTableEntries[] =
	{
		{0.0, 60, 0, "", 1},
		{0.0, 80, 0, "", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};
static IviRangeTable attrVoltageLevelRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltageLevelRangeTableEntries,
	};
    
static ViStatus _VI_FUNC QPXAttrVoltageLevel_WriteCallback (ViSession vi, 						
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  ViInt32		trackingMode;
  ViInt32		retCnt;
  float		ratio;
  int 		range2;
  
  
  char Ret[15];
	

  viCheckErr( viPrintf (io, "CONFIG?\n"));
  viCheckErr( viRead (io, Ret, 14, &retCnt));
  sscanf(Ret,"%d",&trackingMode);     
	
  
  if( (trackingMode != 2) && ( strcmp(channelName , "2") == 0 ) )
	 viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must be in Dual Output Unlinked mode to set V2");	
  
  
  if( (trackingMode == 0 ) )
  {
	  viCheckErr( viPrintf (io, "RATIO?\n"));
  	  viCheckErr( viRead (io, Ret, 14, &retCnt));
	  sscanf(Ret,"%f",&ratio); 
	  
	  viCheckErr( viPrintf (io, "VRANGE2?\n"));
  	  viCheckErr( viRead (io, Ret, 14, &retCnt));
	  sscanf(Ret,"%d",&range2); 
	  
	  if( (range2 == 1) && ( (value * ratio) > 6000 )   )
	  		viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Ratio Mode - V2 voltage exceeds maximum possible for 60V range");
			   
	  if( (range2 == 2) && ( (value * ratio) > 8000 )   )
	  		viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Ratio Mode - V2 voltage exceeds maximum possible for 80V range");		   
	  
	  
  }
  
  

  if (VerifyVoltage)
  	viCheckErr( viPrintf (io, "V%sV %lf\n", channelName, value));
  else
  	viCheckErr( viPrintf (io, "V%s %lf\n", channelName, value));
  
  
  if( (trackingMode == 3) || (trackingMode == 4)   )
  {
  	   if (VerifyVoltage)
  			viCheckErr( viPrintf (io, "V%sV %lf\n", "2", value));
  	   else
  			viCheckErr( viPrintf (io, "V%s %lf\n", "2", value));
  
  }
  
  

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value)
{
  char Cmd[20], CmdRet[3];
  ViStatus error = VI_SUCCESS;
  

  sprintf ( Cmd, "V%s?\n", channelName);
  viCheckErr( viQueryf (io, Cmd, "%c%c%c%lf", &CmdRet[0], &CmdRet[1], &CmdRet[2], value));
  

Error:
  return error;
}

/*- QPX_ATTR_OVP_ENABLED -*/

/*- QPX_ATTR_OVP_LIMIT -*/
static IviRangeTableEntry attrOVPLimitRangeTableEntries[] =
	{
		{2, 90, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};
static IviRangeTable attrOVPLimitRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrOVPLimitRangeTableEntries,
	};

static ViStatus _VI_FUNC QPXAttrOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "OVP%s %lf\n", channelName, value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrOVPLimit_ReadCallback (ViSession vi, 
                                                         ViSession io,
                                                         ViConstString channelName, 
                                                         ViAttr attributeId,
                                                         ViReal64* value)
{
  char Cmd[10];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "OVP%s?\n", channelName);
  viCheckErr( viQueryf (io, Cmd, "%lf", value));

Error:
  return error;
}

/*- QPX_ATTR_CURRENT_LIMIT_BEHAVIOR -*/

/*=PTP KCS:===============================================================*

  NOTE TO THE DEVELOPER: As the PSU only supports regulate the following 
  changes have been made:
  
  - Delete the QPXAttrCurrentLimitBehavior_ReadCallback
    and QPXAttrCurrentLimitBehavior_WriteCallback
    function prototypes and definitions.
  
  - Pass the instrument's fixed value as the default value 
    parameter of the Ivi_AddAttribute function that creates this attribute.

  - Modify the attrCurrentLimitBehaviorRangeTable range table so that
    the only valid value is the current limit behavior your instrument
    supports.
  
*============================================================END=PTP KCS*/

static IviRangeTableEntry attrCurrentLimitBehaviorRangeTableEntries[] =
{
	{QPX_VAL_CURRENT_TRIP, 0, 0, "TRP", 0},
	{IVI_RANGE_TABLE_LAST_ENTRY}
};
static IviRangeTable attrCurrentLimitBehaviorRangeTable =
{
	IVI_VAL_DISCRETE,
  VI_FALSE,
  VI_FALSE,
  VI_NULL,
  attrCurrentLimitBehaviorRangeTableEntries,
};
    


/*- QPX_ATTR_CURRENT_LIMIT -*/
static IviRangeTableEntry attrCurrentLimitRangeTableEntries[] =
	{
		{0.0, 50, 0, "", QPX_VAL_RANGE_1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};
static IviRangeTable attrCurrentLimitRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrentLimitRangeTableEntries,
	};

static ViStatus _VI_FUNC QPXAttrCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  ViInt32		trackingMode;
  ViInt32		retCnt;
  
  char Ret[15];
	

	viCheckErr( viPrintf (io, "CONFIG?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%d",&trackingMode);     
	
	
  
    if(   ((trackingMode == 3) || (trackingMode == 4)) &&  ( strcmp(channelName , "2") == 0 ) )
	 viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must be in Dual Output Unlinked or Ratio mode to set I2");
  
  viCheckErr( viPrintf (io, "I%s %lf\n", channelName, value));
  
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrCurrentLimit_ReadCallback (ViSession vi, 
                                                             ViSession io,
                                                             ViConstString channelName, 
                                                             ViAttr attributeId,
                                                             ViReal64* value)
{
  char Cmd[10], CmdRet[3];
  ViStatus error = VI_SUCCESS;
  
	sprintf ( Cmd, "I%s?\n", channelName);
  viCheckErr( viQueryf (io, Cmd, "%c%c%c%lf", &CmdRet[0], &CmdRet[1], &CmdRet[2], value));
  

Error:
  return error;
}
																		



static ViStatus _VI_FUNC QPXAttrOCPLimit_ReadCallback (ViSession vi,
                                                      ViSession io,
                                                      ViConstString channelName,
                                                      ViAttr attributeId,
                                                      ViReal64 *value)
{
  char Cmd[10];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "OCP%s?\n", channelName);
  viCheckErr( viQueryf (io, Cmd, "%lf", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrOCPLimit_WriteCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViReal64 value){
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "OCP%s %lf\n", channelName, value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrIflock_ReadCallback (ViSession vi, ViSession io,
                                                     ViConstString channelName,
                                                     ViAttr attributeId,
                                                     ViBoolean *value)
{
	char Ret[100];
	
	ViStatus	error = VI_SUCCESS;
	
  viCheckErr( viQueryf (io, "IFLOCK?\n", "%s", Ret));
  if (FindPattern (Ret, 0, -1, "1", 0, 0) != -1)
  	*value = 1;
  else
  	*value = 0;

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrIflock_WriteCallback (ViSession vi, ViSession io,
                                                      ViConstString channelName,
                                                      ViAttr attributeId,
                                                      ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;

  if (value)
  	viCheckErr( viPrintf (io, "IFLOCK\n"));
  else
  	viCheckErr( viPrintf (io, "IFUNLOCK\n"));

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrVoltageStepSize_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
  char Cmd[20], CmdRet[10];
  ViStatus error = VI_SUCCESS;
 
   
  sprintf ( Cmd, "DELTAV%s?\n", channelName);
  viCheckErr( viQueryf (io, Cmd, "%c%c%c%c%c%c%c%c%lf", &CmdRet[0], &CmdRet[1], &CmdRet[2], &CmdRet[3], &CmdRet[4], &CmdRet[5], &CmdRet[6], &CmdRet[7], value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrVoltageStepSize_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  ViInt32		trackingMode;  
  ViInt32		retCnt;
  
   char Ret[15];
	

	viCheckErr( viPrintf (io, "CONFIG?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%d",&trackingMode);     
	
	
  
    if( (trackingMode != 2) && ( strcmp(channelName , "2") == 0 ) )
	 viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must be in Dual Output Unlinked mode to set V2 step size");	
  
  
  
  
  
  viCheckErr( viPrintf (io, "DELTAV%s %lf\n", channelName, value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrCurrentStepSize_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value){
  char Cmd[20], CmdRet[10];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "DELTAI%s?\n", channelName);
  viCheckErr( viQueryf (io, Cmd, "%c%c%c%c%c%c%c%c%lf", &CmdRet[0], &CmdRet[1], &CmdRet[2], &CmdRet[3], &CmdRet[4], &CmdRet[5], &CmdRet[6], &CmdRet[7], value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrCurrentStepSize_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "DELTAI%s %lf\n", channelName, value));
  
Error:
  return error;
}



static ViStatus _VI_FUNC QPXAttrLimitStatusRegister1_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value)
{
  char Cmd[20];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "LSR%s?\n", "1");
  viCheckErr( viQueryf (io, Cmd, "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister1_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value)
{
  char Cmd[20];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "LSE%s?\n", "1");
  viCheckErr( viQueryf (io, Cmd, "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister1_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "LSE%s %ld\n", "1", value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrStandardEventEnableRegister_ReadCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*ESE?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrStandardEventEnableRegister_WriteCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "*ESE %ld\n", value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrIstLocalMessage_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*IST?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrStandardEventStatusRegister_ReadCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*ESR?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrOperationCompleteStatus_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*OPC?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrOperationCompleteStatus_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "*OPC\n"));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrParallelPoleEnableRegister_ReadCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*PRE?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrParallelPoleEnableRegister_WriteCallback (ViSession vi,
                                                                         ViSession io,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViInt32 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "*PRE%ld\n", value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrServiceRequestEnableRegister_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*SRE?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrServiceRequestEnableRegister_WriteCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "*SRE%ld\n", value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrStatusByteRegister_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "*STB?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrAddress_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viQueryf (io, "ADDRESS?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrVerifyVoltage_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean *value)
{
	ViStatus	error = VI_SUCCESS;
	*value = VerifyVoltage;
Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrVerifyVoltage_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	VerifyVoltage = value;

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrOutputEnabledAll_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
  ViStatus error = VI_SUCCESS;
	ViInt32 Channels;
  
	//findout which model we are
	QPX_GetAttributeViInt32 (vi, NULL, QPX_ATTR_NUM_CHANNELS, &Channels);
	if (Channels < 2)
		viCheckErr(IVI_ERROR_FUNCTION_NOT_SUPPORTED);
	

  
  viCheckErr( viPrintf (io, "OPALL %hd\n", value ? 1 : 0));
  
Error:
  return error;
}


static ViStatus _VI_FUNC QPXAttrLimitStatusRegister2_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value){
  char Cmd[20];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "LSR%s?\n", "2");
  viCheckErr( viQueryf (io, Cmd, "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister2_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value){
  char Cmd[20];
  ViStatus error = VI_SUCCESS;
  
  sprintf ( Cmd, "LSE%s?\n", "2");
  viCheckErr( viQueryf (io, Cmd, "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrLimitStatusEnableRegister2_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value){
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "LSE%s %ld\n", "2", value));
  
Error:
  return error;
}


static ViStatus _VI_FUNC QPXAttrDamping_WriteCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViBoolean value){
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "DAMPING%s %hd\n", channelName, value ? 1 : 0));
  
Error:
  return error;
}







	  

static ViStatus _VI_FUNC QPXAttrRemoteSenseMode_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViBoolean value)
{
  ViStatus	error = VI_SUCCESS;
	
  if (value)
  	viCheckErr( viPrintf (io, "SENSE%s 1\n", channelName, value));
  else
  	viCheckErr( viPrintf (io, "SENSE%s 0\n", channelName, value));

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrRangeSetting_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value)
{
char Cmd[10], CmdRet[100];
  ViStatus error = VI_SUCCESS;
  
  if(PSUModel == QPX600)
  {
  	sprintf ( Cmd, "VRANGE%s?\n", channelName);
  	viCheckErr( viQueryf (io, Cmd, "%i", CmdRet));
  	*value = CmdRet[0];
  }
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrRangeSetting_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value)
{
 char Cmd[100];
	ViStatus error = VI_SUCCESS;
	
	ViReal64	setVolts_1;
	ViReal64	setVolts_2;
	ViInt32		retCnt;
	char Ret[15];  
	int channel;

	// check when stepping down voltage range that set volts do not exceed low range maximum
	
	if(value == 1)
	{
		viCheckErr( viPrintf (io, "V1?\n"));
		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%*s%lf",&setVolts_1);   
		
		viCheckErr( viPrintf (io, "V2?\n"));
		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%*s%lf",&setVolts_2);
		
		channel = atoi(channelName);   
		
		if (channel == 1)
		{
			if(setVolts_1 > 60)
			  viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Set volts too high for requested range");
		}
		if (channel == 2)
		{
			if(setVolts_2 > 60)
			  viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Set volts too high for requested range");
		}
		
		
		
	}
	
	
	//sprintf ( Cmd, "VRANGE%s %d\n", channelName, value);
  	//viCheckErr( viPrintf (io, Cmd));
	
	viCheckErr( viPrintf (io, "VRANGE%s %d\n",channelName, value));             
 
  
Error:
  return error;
}

static ViStatus _VI_FUNC QPXAttrRatio_ReadCallback (ViSession vi, ViSession io,
                                                    ViConstString channelName,
                                                    ViAttr attributeId,
                                                    ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrRatio_WriteCallback (ViSession vi, ViSession io,
                                                     ViConstString channelName,
                                                     ViAttr attributeId,
                                                     ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32		trackingMode;
	ViReal64	setVolts_1;
	ViReal64	setVolts_2;
	ViInt32		retCnt;
	
	
	char Ret[15];
	int  vRange_2;

	viCheckErr( viPrintf (io, "CONFIG?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%d",&trackingMode);     
	
	
	
	if(trackingMode != 0)
	    viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must be in Ratio Tracking Mode to use this function");
	

	viCheckErr( viPrintf (io, "V1?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%*s%lf",&setVolts_1);     
	
	
	
/*	
	if(setVolts_1 < 1)
	   viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "V1 and V2 must be at least 1V ");
*/	

	viCheckErr( viPrintf (io, "V2?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%*s%lf",&setVolts_2);     
	
/*	
	if(setVolts_2 < 1)
	   viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "V1 and V2 must be at least 1V ");	
*/	
	
	
	viCheckErr( viPrintf (io, "VRANGE2?\n"));
	viCheckErr( viRead (io, Ret, 14, &retCnt));
	sscanf(Ret,"%d",&vRange_2);     
	
	if(vRange_2 == 1)
		if(  (value * setVolts_1) > 6000)
		   viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Ratio exceeds volts range limit for Channel 2");
	
	if(vRange_2 == 2)
		if(  (value * setVolts_1) > 8000)
		   viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Ratio exceeds volts range limit for Channel 2");
	
	
	
    viCheckErr( viPrintf (io, "RATIO %lf\n", value));               

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrTrackingMode_ReadCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrTrackingMode_WriteCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	ViInt32		op1_On;
	ViInt32		op2_On;
	ViReal64	setVolts_1;
	ViReal64	setVolts_2;
	
	ViInt32		rangeOP_1;
	ViReal64	setAmps_1;
	ViReal64	ovp_1;
	ViReal64	ocp_1;
	
	ViInt32		retCnt;
	ViInt32		counter = 0;
	
	char Ret[15];  
	char Ret1[15];
	char ignore[15];
	
	if( (value == 3) || (value == 4)  )
	{
	
		viCheckErr( viPrintf (io, "OP1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%d",&op1_On);   
		
		
		viCheckErr( viPrintf (io, "OP2?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%d",&op2_On);   
		
		
		if ( (op1_On == 1) || (op2_On == 1)  )
			viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
          	     "Output 1 and Output 2 must be off to enter this mode");
		
	
		// Need to get OP1 RANGE, VOLTS, CURRENT, OVP, OCP and set OP2 with them
	
	
		viCheckErr( viPrintf (io, "VRANGE1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%d",&rangeOP_1);   
		
		viCheckErr( viPrintf (io, "V1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"V1 %lf",&setVolts_1);     
		
		viCheckErr( viPrintf (io, "I1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"I1 %lf",&setAmps_1);  
		
		viCheckErr( viPrintf (io, "OVP1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"VP1 %lf",&ovp_1);     
		
		viCheckErr( viPrintf (io, "OCP1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"CP1 %lf",&ocp_1);     
		
		
		viCheckErr( viPrintf (io, "VRANGE2 %d\n", rangeOP_1)); 
		viCheckErr( viPrintf (io, "V2 %lf\n", setVolts_1));
		viCheckErr( viPrintf (io, "I2 %lf\n", setAmps_1));
		viCheckErr( viPrintf (io, "OVP2 %lf\n", ovp_1));
		viCheckErr( viPrintf (io, "OCP2 %lf\n", ocp_1));
		
		
		viCheckErr( viPrintf (io, "CONFIG %d\n", value));   
		
	}	
	
	
	if(value == 0)
	{
		viCheckErr( viPrintf (io, "V1?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%*s%lf",&setVolts_1);     
	
		if(setVolts_1 < 1)
	 	  viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "V1 and V2 must be at least 1V ");
	
		viCheckErr( viPrintf (io, "V2?\n"));
 		viCheckErr( viRead (io, Ret, 14, &retCnt));
		sscanf(Ret,"%*s%lf",&setVolts_2);     
	
		if(setVolts_2 < 1)
	   	viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "V1 and V2 must be at least 1V ");		
	
		viCheckErr( viPrintf (io, "CONFIG %lf\n", value));    
	
	
	}
	
	if(value == 2)
	{
		viCheckErr( viPrintf (io, "CONFIG %d\n", value));    
	}	
	

Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrVoltageLevel_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	ViReal64			min, max; 
	IviRangeTablePtr	rangeTable = &attrVoltageLevelRangeTable;
	
	checkErr( Ivi_GetAttrRangeTable(vi, channelName, attributeId, &rangeTable));             
	
	viCheckErr( Ivi_GetViReal64EntryFromCmdValue(1, rangeTable, &min, &max,                                                                            
    											   VI_NULL, VI_NULL, VI_NULL));        
	
	
	
	viCheckErr( Ivi_CheckNumericRange(value, min, max, IVI_ERROR_INVALID_VALUE));    
	
	
Error:
	return error;
}

static ViStatus _VI_FUNC QPXAttrVoltageLevel_RangeTableCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 IviRangeTablePtr *rangeTablePtr)
{
	ViStatus	error = VI_SUCCESS;
	IviRangeTablePtr	tblPtr = VI_NULL;

	IviRangeTablePtr	rangeTable = VI_NULL;   
	ViReal64			min, max;
	ViInt32             level;  
	
	
Error:
//	*rangeTablePtr = tblPtr;
	*rangeTablePtr = &attrVoltageLevelRangeTable;
	return error;
}

/*****************************************************************************
 * Function: QPX_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus QPX_InitAttributes (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;
	ViInt32     flags = 0;
	
//	ViChar              rdbuffer[BUFFER_SIZE];  
	
	    /*=PTP KCS:=============================================================*

	        NOTE TO THE DEVELOPER:  You can add additional parameters to the
	        prototype of this function.  This is useful when you want to pass
	        information from the initialization functions.  The Attribute Editor 
	        in LabWindows/CVI requires that the name of this function be 
	        QPX_InitAttributes.

	     *==========================================================END=PTP KCS=*/

	    /*- Initialize instrument attributes --------------------------------*/            

	checkErr( Ivi_SetAttributeViInt32 (vi, "", QPX_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
	                                   0, QPX_MAJOR_VERSION));
	checkErr( Ivi_SetAttributeViInt32 (vi, "", QPX_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
	                                   0, QPX_MINOR_VERSION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, QPX_ATTR_SPECIFIC_DRIVER_REVISION,
	                                           QPXAttrDriverRevision_ReadCallback));
	checkErr( Ivi_SetAttributeViInt32 (vi, "", QPX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
	                                   0, QPX_CLASS_SPEC_MAJOR_VERSION));
	checkErr( Ivi_SetAttributeViInt32 (vi, "", QPX_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
	                                   0, QPX_CLASS_SPEC_MINOR_VERSION));
	checkErr( Ivi_SetAttributeViString (vi, "", QPX_ATTR_IO_SESSION_TYPE,
	                                    0, QPX_IO_SESSION_TYPE));
	checkErr( Ivi_SetAttributeViString (vi, "", QPX_ATTR_SUPPORTED_INSTRUMENT_MODELS,
	                                    0, QPX_SUPPORTED_INSTRUMENT_MODELS));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, QPX_ATTR_INSTRUMENT_FIRMWARE_REVISION,
	                                           QPXAttrFirmwareRevision_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, QPX_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, QPX_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
	                               flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_GetAttributeFlags (vi, QPX_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, QPX_ATTR_INSTRUMENT_MANUFACTURER, 
	                               flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, QPX_ATTR_INSTRUMENT_MANUFACTURER,
	                                         QPXAttrInstrumentManufacturer_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, QPX_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, QPX_ATTR_INSTRUMENT_MODEL, 
	                               flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, QPX_ATTR_INSTRUMENT_MODEL,
	                                         QPXAttrInstrumentModel_ReadCallback));
	checkErr( Ivi_SetAttributeViString (vi, "", QPX_ATTR_SPECIFIC_DRIVER_VENDOR,
	                                    0, QPX_DRIVER_VENDOR));
	checkErr( Ivi_SetAttributeViString (vi, "", QPX_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
	                                    0, QPX_DRIVER_DESCRIPTION));
	checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
	                                  QPX_WaitForOPCCallback));
	checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
	                                  QPX_CheckStatusCallback));
	checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE, 
	                                   0, VI_FALSE));
	checkErr( Ivi_SetAttributeViString (vi, "", QPX_ATTR_GROUP_CAPABILITIES, 0,
	                                  "IviDCPwrBase,"
	                                  "IviDCPwrTrigger,"
	                                  "IviDCPwrSoftwareTrigger,"
	                                  "IviDCPwrMeasurement"));

	    /*- Add instrument-specific attributes ------------------------------*/            
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_OUTPUT_ENABLED,
	                                   "QPX_ATTR_OUTPUT_ENABLED", VI_FALSE,
	                                   IVI_VAL_MULTI_CHANNEL, VI_NULL,
	                                   QPXAttrOutputEnabled_WriteCallback));
  
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_VOLTAGE_LEVEL,
	                                    "QPX_ATTR_VOLTAGE_LEVEL", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    QPXAttrVoltageLevel_ReadCallback,
	                                    QPXAttrVoltageLevel_WriteCallback,
	                                    &attrVoltageLevelRangeTable, 0));
	checkErr (Ivi_SetAttrRangeTableCallback (vi, QPX_ATTR_VOLTAGE_LEVEL,
	                                         QPXAttrVoltageLevel_RangeTableCallback));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, QPX_ATTR_VOLTAGE_LEVEL,
	                                            QPXAttrVoltageLevel_CheckCallback));
                                       
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_OVP_ENABLED,
	                                     "QPX_ATTR_OVP_ENABLED", VI_TRUE,
	                                     IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL));
  
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_OVP_LIMIT, "QPX_ATTR_OVP_LIMIT",
	                                    65, IVI_VAL_MULTI_CHANNEL,
	                                    QPXAttrOVPLimit_ReadCallback,
	                                    QPXAttrOVPLimit_WriteCallback,
	                                    &attrOVPLimitRangeTable, 0));
  
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_CURRENT_LIMIT_BEHAVIOR,
	                                 "QPX_ATTR_CURRENT_LIMIT_BEHAVIOR",
	                                 QPX_VAL_CURRENT_TRIP,
	                                 IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL,
	                                 &attrCurrentLimitBehaviorRangeTable));
  
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_CURRENT_LIMIT,
	                                    "QPX_ATTR_CURRENT_LIMIT", 1.00,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    QPXAttrCurrentLimit_ReadCallback,
	                                    QPXAttrCurrentLimit_WriteCallback,
	                                    &attrCurrentLimitRangeTable, 0));
  
	checkErr (Ivi_AddAttributeViString (vi, QPX_ATTR_ID_QUERY_RESPONSE,
	                                    "QPX_ATTR_ID_QUERY_RESPONSE",
	                                    "THURLBY THANDAR, QPX600DP, 0, 0.00",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    QPXAttrIdQueryResponse_ReadCallback, VI_NULL));
                                         
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_OPC_TIMEOUT,
	                                   "QPX_ATTR_OPC_TIMEOUT", 5000,
	                                   IVI_VAL_DONT_CHECK_STATUS | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrOpcTimeoutRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_OCP_LIMIT, "QPX_ATTR_OCP_LIMIT",
	                                    55, IVI_VAL_MULTI_CHANNEL,
	                                    QPXAttrOCPLimit_ReadCallback,
	                                    QPXAttrOCPLimit_WriteCallback,
	                                    &attrOCPLimitRangeTable, 0));
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_IFLOCK, "QPX_ATTR_IFLOCK",
	                                     VI_FALSE, 0, QPXAttrIflock_ReadCallback,
	                                     QPXAttrIflock_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_VOLTAGE_STEP_SIZE,
	                                    "QPX_ATTR_VOLTAGE_STEP_SIZE", 0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    QPXAttrVoltageStepSize_ReadCallback,
	                                    QPXAttrVoltageStepSize_WriteCallback,
	                                    &attrVoltageLevelRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_CURRENT_STEP_SIZE,
	                                    "QPX_ATTR_CURRENT_STEP_SIZE", 0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    QPXAttrCurrentStepSize_ReadCallback,
	                                    QPXAttrCurrentStepSize_WriteCallback,
	                                    &attrCurrentLimitRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_LIMIT_STATUS_REGISTER_1,
	                                   "QPX_ATTR_LIMIT_STATUS_REGISTER_1", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_MULTI_CHANNEL,
	                                   QPXAttrLimitStatusRegister1_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_LIMIT_STATUS_ENABLE_REGISTER_1,
	                                   "QPX_ATTR_LIMIT_STATUS_ENABLE_REGISTER_1", 0,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   QPXAttrLimitStatusEnableRegister1_ReadCallback,
	                                   QPXAttrLimitStatusEnableRegister1_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_STANDARD_EVENT_ENABLE_REGISTER,
	                                   "QPX_ATTR_STANDARD_EVENT_ENABLE_REGISTER", 0,
	                                   0,
	                                   QPXAttrStandardEventEnableRegister_ReadCallback,
	                                   QPXAttrStandardEventEnableRegister_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_STANDARD_EVENT_STATUS_REGISTER,
	                                   "QPX_ATTR_STANDARD_EVENT_STATUS_REGISTER", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   QPXAttrStandardEventStatusRegister_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_IST_LOCAL_MESSAGE,
	                                   "QPX_ATTR_IST_LOCAL_MESSAGE", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   QPXAttrIstLocalMessage_ReadCallback, VI_NULL,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_OPERATION_COMPLETE_STATUS,
	                                   "QPX_ATTR_OPERATION_COMPLETE_STATUS", 0, 0,
	                                   QPXAttrOperationCompleteStatus_ReadCallback,
	                                   QPXAttrOperationCompleteStatus_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_PARALLEL_POLE_ENABLE_REGISTER,
	                                   "QPX_ATTR_PARALLEL_POLE_ENABLE_REGISTER", 0,
	                                   0,
	                                   QPXAttrParallelPoleEnableRegister_ReadCallback,
	                                   QPXAttrParallelPoleEnableRegister_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_SERVICE_REQUEST_ENABLE_REGISTER,
	                                   "QPX_ATTR_SERVICE_REQUEST_ENABLE_REGISTER",
	                                   0, 0,
	                                   QPXAttrServiceRequestEnableRegister_ReadCallback,
	                                   QPXAttrServiceRequestEnableRegister_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_STATUS_BYTE_REGISTER,
	                                   "QPX_ATTR_STATUS_BYTE_REGISTER", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   QPXAttrStatusByteRegister_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_ADDRESS, "QPX_ATTR_ADDRESS", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   QPXAttrAddress_ReadCallback, VI_NULL,
	                                   &attrAddressRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_VERIFY_VOLTAGE,
	                                     "QPX_ATTR_VERIFY_VOLTAGE", VI_FALSE,
	                                     IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                     QPXAttrVerifyVoltage_ReadCallback,
	                                     QPXAttrVerifyVoltage_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_OUTPUT_ENABLED_ALL,
	                                   "QPX_ATTR_OUTPUT_ENABLED_ALL", VI_FALSE, 0,
	                                   VI_NULL,
	                                   QPXAttrOutputEnabledAll_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_LIMIT_STATUS_REGISTER_2,
	                                   "QPX_ATTR_LIMIT_STATUS_REGISTER_2", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_MULTI_CHANNEL,
	                                   QPXAttrLimitStatusRegister2_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_LIMIT_STATUS_ENABLE_REGISTER_2,
	                                   "QPX_ATTR_LIMIT_STATUS_ENABLE_REGISTER_2", 0,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   QPXAttrLimitStatusEnableRegister2_ReadCallback,
	                                   QPXAttrLimitStatusEnableRegister2_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_DAMPING, "QPX_ATTR_DAMPING",
	                                     VI_FALSE,
	                                     IVI_VAL_NOT_USER_READABLE | IVI_VAL_MULTI_CHANNEL,
	                                     VI_NULL, QPXAttrDamping_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, QPX_ATTR_REMOTE_SENSE_MODE,
	                                     "QPX_ATTR_REMOTE_SENSE_MODE", VI_FALSE,
	                                     IVI_VAL_NOT_USER_READABLE | IVI_VAL_MULTI_CHANNEL,
	                                     VI_NULL,
	                                     QPXAttrRemoteSenseMode_WriteCallback));
	
/*	
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_RANGE_SETTING,
	                                   "QPX_ATTR_RANGE_SETTING", 1,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                   QPXAttrRangeSetting_ReadCallback,
	                                   QPXAttrRangeSetting_WriteCallback,
	                                   &attrRangeSettingRangeTable));
*/	
	
	checkErr (Ivi_AddAttributeViReal64 (vi, QPX_ATTR_RATIO, "QPX_ATTR_RATIO", 100, 0,
	                                    QPXAttrRatio_ReadCallback,
	                                    QPXAttrRatio_WriteCallback,
	                                    &attrRatioRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_TRACKING_MODE,
	                                   "QPX_ATTR_TRACKING_MODE", 1, 0,
	                                   QPXAttrTrackingMode_ReadCallback,
	                                   QPXAttrTrackingMode_WriteCallback,
	                                   &attrTrackingModeRangeTable));
	
	
	checkErr (Ivi_AddAttributeViInt32 (vi, QPX_ATTR_RANGE_SETTING,
	                                   "QPX_ATTR_RANGE_SETTING", 0,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   QPXAttrRangeSetting_ReadCallback,
	                                   QPXAttrRangeSetting_WriteCallback,
	                                   &attrRangeSettingRangeTable));
	
	
	
                                   
	/*- Setup attribute invalidations -----------------------------------*/            

	checkErr (Ivi_AddAttributeInvalidation (vi, QPX_ATTR_VERIFY_VOLTAGE,
	                                      QPX_ATTR_VOLTAGE_LEVEL, VI_TRUE));
	
	
	checkErr (Ivi_AddAttributeInvalidation (vi, QPX_ATTR_RANGE_SETTING,
	                                        QPX_ATTR_VOLTAGE_LEVEL, VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, QPX_ATTR_RANGE_SETTING,
	                                        QPX_ATTR_CURRENT_LIMIT, VI_TRUE));
	
	
	
	Error:
	return error;
}

static ViStatus QPX_InitRangeTables (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;
	ViChar rdBuffer[BUFFER_SIZE];

	//customise the range table to the specific model, note that the default is the QPX1200
	checkErr( Ivi_GetAttributeViString (vi, VI_NULL, QPX_ATTR_ID_QUERY_RESPONSE, 0, BUFFER_SIZE, rdBuffer));

	if (error == VI_SUCCESS)
	{
 	  switch (PSUModel)
		{
			case QPX1200:
			default:	
				attrVoltageLevelRangeTableEntries[0].maxValue = 60.00;
			//	attrOVPLimitRangeTableEntries[0].maxValue = 65.00;     
			//	attrVoltageLevelRangeTableEntries[1].maxValue = 15.00;
			//	attrVoltageLevelRangeTableEntries[2].maxValue = 15.00;
			//	attrCurrentLimitRangeTableEntries[0].maxValue = 50.0;
			//	attrCurrentLimitRangeTableEntries[1].maxValue = 5.0;
				break;
			case QPX600:
				attrVoltageLevelRangeTableEntries[0].maxValue = 80.00;
			//	attrOVPLimitRangeTableEntries[1].maxValue = 90.00;
			//	attrVoltageLevelRangeTableEntries[1].maxValue = 80.00;
			//	attrVoltageLevelRangeTableEntries[2].maxValue = 30.00;
			//	attrCurrentLimitRangeTableEntries[0].maxValue = 50.0;
			//	attrCurrentLimitRangeTableEntries[1].maxValue = 3.0;
				break;

		}
	}

	Error:
	return error;
}


/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling 
 *
 *****************************************************************************/

