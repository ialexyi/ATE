#include <ansi_c.h>
#include <utility.h>


/*****************************************************************************
 *  Thurlby-Thandar CPX400 Series PSU Instrument Driver                               
 *  LabWindows/CVI 8.5 Instrument Driver                                     
 *                                                                  
 *  
 *		By: Philip Hoole, TTi Ltd                              
 *      PH. 44 1480 412451   Fax 44 1480 450409            
 *
 *		Modification History:                                                    
 *                                                                           
 *      Wednesday, April 21, 2010 - Instrument Driver Created.  
 *
 *		v1.0 May 13,2010 - Initial Release
 *
 *		v1.1 May 11,2011 - Added support for single channel models - CPX400SP and XPF60-20SP
 *
 *		v1.2 September,2012 - Fixed virtual channel name coercion
 *
 *                                                                           
 *****************************************************************************/

#include <userint.h>
#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include <ctype.h>
#include "CPX400.h"

static ViBoolean VerifyVoltage = VI_FALSE;
PSUModel_t PSUModel = CPX400DP;
/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define CPX400_ATTR_OPC_TIMEOUT      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)   /* ViInt32 */

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        
#define CPX400_IO_SESSION_TYPE   IVI_VAL_VISA_SESSION_TYPE

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT   0x08
#define IEEE_488_2_EXECUTION_ERROR_BIT          0x10
#define IEEE_488_2_COMMAND_ERROR_BIT            0x20

    /*- List of channels passed to the Ivi_BuildChannelTable function -------*/ 
//#define CHANNEL_LIST                            "1, 2"	 //default to one, modified by CPX400_IviInt
#define CHANNEL_LIST                            "1,2"	 //default to two

static IviRangeTableEntry attrRatioRangeTableEntries[] =
	{
		{0, 100, 0, "", 0},
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

static IviRangeTableEntry attrConfigRangeTableEntries[] =
	{
		{0, 0, 0, "", 0},
		{2, 0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfigRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfigRangeTableEntries,
	};

static IviRangeTableEntry attrAddressRangeTableEntries[] =
	{
		{1, 31, 0, "", 0},
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
		{0, 21, 0, "", 0},
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
		{1, 0, 0, "", CPX400_VAL_RANGE_2},
		{2, 0, 0, "", CPX400_VAL_RANGE_3},
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
static ViStatus CPX400_InitRangeTables (ViSession vi);
static ViStatus CPX400_InitAttributes (ViSession vi);
static ViStatus CPX400_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus CPX400_CheckStatus (ViSession vi);
static ViStatus CPX400_WaitForOPC (ViSession vi, ViInt32 maxTime);
static ViStatus CPX400_GetDriverSetupOption (ViSession vi,
                                               ViConstString optionTag,
                                               ViInt32 optionValueSize,
                                               ViChar optionValue[]);

    /*- File I/O Utility Functions -*/
static ViStatus CPX400_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus CPX400_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC CPX400_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC CPX400_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC CPX400AttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC CPX400AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC CPX400AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC CPX400AttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC CPX400AttrOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);
static ViStatus _VI_FUNC CPX400AttrVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC CPX400AttrVoltageLevel_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);
static ViStatus _VI_FUNC CPX400AttrOVPLimit_WriteCallback (ViSession vi, 
                                                            ViSession io,
                                                            ViConstString channelName, 
                                                            ViAttr attributeId,
                                                            ViReal64 value);
static ViStatus _VI_FUNC CPX400AttrOVPLimit_ReadCallback (ViSession vi, 
                                                           ViSession io,
                                                           ViConstString channelName, 
                                                           ViAttr attributeId,
                                                           ViReal64* value);
static ViStatus _VI_FUNC CPX400AttrCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC CPX400AttrCurrentLimit_ReadCallback (ViSession vi, 
                                                               ViSession io,
                                                               ViConstString channelName, 
                                                               ViAttr attributeId,
                                                               ViReal64* value);

static ViStatus _VI_FUNC CPX400AttrRangeSetting_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrRangeSetting_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrOCPLimit_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 *value);

static ViStatus _VI_FUNC CPX400AttrOCPLimit_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value);

static ViStatus _VI_FUNC CPX400AttrIflock_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViBoolean *value);

static ViStatus _VI_FUNC CPX400AttrIflock_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViBoolean value);

static ViStatus _VI_FUNC CPX400AttrVoltageStepSize_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC CPX400AttrVoltageStepSize_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC CPX400AttrCurrentStepSize_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC CPX400AttrCurrentStepSize_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);



static ViStatus _VI_FUNC CPX400AttrLimitStatusRegister1_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister1_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister1_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrStandardEventEnableRegister_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrStandardEventEnableRegister_WriteCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrIstLocalMessage_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrStandardEventStatusRegister_ReadCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrOperationCompleteStatus_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrOperationCompleteStatus_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrParallelPoleEnableRegister_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrParallelPoleEnableRegister_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrServiceRequestEnableRegister_ReadCallback (ViSession vi,
                                                                            ViSession io,
                                                                            ViConstString channelName,
                                                                            ViAttr attributeId,
                                                                            ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrServiceRequestEnableRegister_WriteCallback (ViSession vi,
                                                                             ViSession io,
                                                                             ViConstString channelName,
                                                                             ViAttr attributeId,
                                                                             ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrStatusByteRegister_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrAddress_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrVerifyVoltage_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViBoolean *value);

static ViStatus _VI_FUNC CPX400AttrVerifyVoltage_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViBoolean value);

static ViStatus _VI_FUNC CPX400AttrOutputEnabledAll_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);


static ViStatus _VI_FUNC CPX400AttrLimitStatusRegister2_ReadCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister2_ReadCallback (ViSession vi,
                                                                          ViSession io,
                                                                          ViConstString channelName,
                                                                          ViAttr attributeId,
                                                                          ViInt32 *value);

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister2_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value);

static ViStatus _VI_FUNC CPX400AttrTrackingMode_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value);


static ViStatus _VI_FUNC CPX400AttrRatio_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViReal64 *value);

static ViStatus _VI_FUNC CPX400AttrRatio_WriteCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 value);

static ViStatus _VI_FUNC CPX400AttrTrackingMode_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);






/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: CPX400_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           CPX400_InitWithOptions function.   
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
	ViStatus    error = VI_SUCCESS;

	if (newVi == VI_NULL)
	{
		Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
		                  VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
		checkErr( IVI_ERROR_INVALID_PARAMETER);
	}

	checkErr( CPX400_InitWithOptions (resourceName, IDQuery, resetDevice, 
	                                "", newVi));

	Error:  
	return error;
}

/*****************************************************************************
 * Function: CPX400_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
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
	checkErr( Ivi_SpecificDriverNew ("CPX400", newOptionString, &vi));  

	/* init the driver */
	checkErr( CPX400_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
	if (isLogicalName)
		checkErr( Ivi_ApplyDefaultSetup (vi));
	*newVi = vi;

	Error:
	if (error < VI_SUCCESS) 
		Ivi_Dispose (vi);

	return error;
}

/*****************************************************************************
 * Function: CPX400_IviInit                                                       
 * Purpose:  This function is called by CPX400_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_IviInit (ViRsrc resourceName, ViBoolean IDQuery, ViBoolean reset, ViSession vi)
{
	ViStatus    error = VI_SUCCESS;
	ViSession   io = VI_NULL;
	ViChar rdBuffer[BUFFER_SIZE];
	ViChar      idQ[BUFFER_SIZE];  
	
	//ViUInt32 BaudRate;

	checkErr( Ivi_BuildChannelTable (vi, CHANNEL_LIST, VI_FALSE, VI_NULL));

	/* Add attributes */
	checkErr( CPX400_InitAttributes (vi));

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
		checkErr( CPX400_reset (vi));
	else  /*- Send Default Instrument Setup ---------------------------------*/
		checkErr( CPX400_DefaultInstrSetup (vi));

	/*- Identification Query ------------------------------------------------*/
	if (IDQuery)                               
	{
		#define MUST_HAVE_IN_RESPONSE_STRING_1		 "THURLBY THANDAR"
		#define MUST_HAVE_IN_RESPONSE_STRING_2		 "CPX"
		#define MUST_HAVE_IN_RESPONSE_STRING_1a		 "SORENSEN"
	//	#define MUST_HAVE_IN_RESPONSE_STRING_2a		 "XPF 60-20DP"
		#define MUST_HAVE_IN_RESPONSE_STRING_2a		 "XPF"     

		checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CPX400_ATTR_ID_QUERY_RESPONSE, 0, BUFFER_SIZE, rdBuffer));

		if (strstr (rdBuffer, MUST_HAVE_IN_RESPONSE_STRING_1) == NULL || strstr (rdBuffer, MUST_HAVE_IN_RESPONSE_STRING_2) == NULL )
		{    
			if (strstr (rdBuffer, MUST_HAVE_IN_RESPONSE_STRING_1a) == NULL || strstr (rdBuffer, MUST_HAVE_IN_RESPONSE_STRING_2a) == NULL )
				viCheckErr( VI_ERROR_FAIL_ID_QUERY);
		}
		
		
		checkErr( Ivi_GetAttributeViString( vi,
                                        VI_NULL,
                                        CPX400_ATTR_ID_QUERY_RESPONSE,
                                        0, 256, idQ));
		
		sscanf (idQ,  "%*[^,],%256[^,]", rdBuffer);
		
		
		
		
		//configure the model
		if (strstr (rdBuffer, "CPX400DP") != NULL)
			PSUModel = CPX400DP;
	
		else if (strstr (rdBuffer, "XPF 60-20DP") != NULL)
			PSUModel = 	XPF6020DP;
	
		else if (strstr (rdBuffer, "CPX400SP") != NULL)
			PSUModel = CPX400SP;

		else if (strstr (rdBuffer, "XPF 60-20SP") != NULL)
			PSUModel = XPF6020SP;
	
		
				
		else
			PSUModel = CPX400DP;
		
		checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CPX400_ATTR_INSTRUMENT_MODEL, 0, BUFFER_SIZE, rdBuffer));
	}

	//setup range tables with model specific information
	checkErr( CPX400_InitRangeTables (vi));
	
	checkErr( CPX400_CheckStatus (vi));

	Error:
	if (error < VI_SUCCESS)
	{
		if (!Ivi_Simulating (vi) && io)
		  viClose (io);
	}
	return error;
}

/*****************************************************************************
 * Function: CPX400_close                                                           
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
ViStatus _VI_FUNC CPX400_close (ViSession vi)
{
  ViStatus    error = VI_SUCCESS;
  
  checkErr( Ivi_LockSession (vi, VI_NULL));
  
  checkErr( CPX400_IviClose (vi));

	Error:    
  Ivi_UnlockSession (vi, VI_NULL);
  Ivi_Dispose (vi);  

  return error;
}
/*****************************************************************************
 * Function: CPX400_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           CPX400_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_IviClose (ViSession vi)
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
 * Function: CPX400_WaitForOperationComplete  
 * Purpose:  Wait for Operation Complete true. As all commands are completely
 *					 executed before the next is started this command takes no
 *					 additional action.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_WaitForOperationComplete (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "*WAI\n"));
	}

	checkErr( CPX400_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_ClearStatus  
 * Purpose:  Clears the Standard Event Status Register, Query Error Register
 *					 and Execution Error Register. This indirectly clears the Status
 *					 Byte Register.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ClearStatus (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	{
		ViSession   io = Ivi_IOSession(vi); /* call only when locked */

		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "*CLS\n"));
	}

	checkErr( CPX400_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_reset (ViSession vi)
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
	checkErr( CPX400_DefaultInstrSetup (vi));                                
	checkErr( CPX400_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_SaveSetup
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
ViStatus _VI_FUNC CPX400_SaveSetup (ViSession vi,
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

	checkErr( CPX400_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_RecallSetup
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
ViStatus _VI_FUNC CPX400_RecallSetup (ViSession vi,
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

	checkErr( CPX400_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_Local
 * Purpose: Switches the instrument to local mode. 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_Local (ViSession vi)
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
	
	//checkErr( CPX400_DefaultInstrSetup (vi));                                

	//checkErr( CPX400_CheckStatus (vi));                                      

	Error:
	Ivi_UnlockSession (vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_self_test (ViSession vi, ViInt16 *testResult, 
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

	checkErr( CPX400_CheckStatus (vi));

	Error:
		Ivi_UnlockSession(vi, VI_NULL);
		return error;
}

/*****************************************************************************
 * Function: CPX400_query_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_error_query (ViSession vi, ViInt32 *errCode, 
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
 * Function: CPX400_execution_error_query                                                     
 * Purpose:  This function queries the instrument execution error queue and    
 *           returns the result.                                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_execution_error_query (ViSession vi, ViInt32 *errCode, 
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
 * Function: CPX400_error_message                                                  
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
ViStatus _VI_FUNC CPX400_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
	ViStatus    error = VI_SUCCESS;

	static      IviStringValueTable errorTable = 
	{
		/*=PTP KCS================================================================*
		    Insert instrument driver specific error codes here.  Example:                        

		    {CPX400_ERROR_TOO_MANY_SAMPLES,   "Sample Count cannot exceed 512."},   
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
 * Function: CPX400_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
  ViStatus    error = VI_SUCCESS;
  
  checkErr( Ivi_LockSession (vi, VI_NULL));

  if (driverRev == VI_NULL)
    viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
  if (instrRev == VI_NULL)
    viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

  checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CPX400_ATTR_SPECIFIC_DRIVER_REVISION, 
                                      0, 256, driverRev));
  checkErr( Ivi_GetAttributeViString (vi, "", CPX400_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                      0, 256, instrRev));
  checkErr( CPX400_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: CPX400_ConfigureOutputEnabled                                   
 * Purpose:  This function controls whether or not the signal the power supply
 *           generates is present at the output terminal.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ConfigureOutputEnabled (ViSession vi,
                                                   ViConstString channelName,
                                                   ViBoolean enabled)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_SetAttributeViBoolean (vi, channelName, CPX400_ATTR_OUTPUT_ENABLED,
                                      IVI_VAL_DIRECT_USER_CALL, enabled);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;
}



/*****************************************************************************
 * Function: CPX400_ConfigureCurrentLimit                                   
 * Purpose:  This function configures the behavior and level of the
 *           current limit.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ConfigureCurrentLimit (ViSession vi,
                                              ViChar channelName[],
                                              ViInt32 behavior,
                                              ViReal64 limit)
{
	ViInt32	range;
	
	ViStatus error = VI_SUCCESS;

	
	Ivi_LockSession(vi, VI_NULL);
	
	
	checkErr(Ivi_GetAttributeViInt32 (vi, channelName, CPX400_ATTR_RANGE_SETTING,
                                       0, &range));
	
	if (behavior == CPX400_VAL_CURRENT_REGULATE || behavior == CPX400_VAL_CURRENT_TRIP)   
		error = Ivi_SetAttributeViReal64 (vi, channelName, CPX400_ATTR_CURRENT_LIMIT, 0, limit);	
	

	Error:    
	
	Ivi_UnlockSession(vi, VI_NULL);

	return  error ;
	
}

/*****************************************************************************
 * Function: CPX400_ConfigureOVP                                   
 * Purpose:  This function enables or disables over-voltage protection and
 *           configures the over-voltage limit.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ConfigureOVP (ViSession vi,
                                         ViConstString channelName,
										 ViBoolean enabled,
                                         ViReal64 limit)
{
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	//If you are enabling over-voltage protection, set the limit first
	//to avoid a premature over-voltage condition.
	if (enabled)
	{
		viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
		                                   CPX400_ATTR_OVP_LIMIT,
		                                   0, limit), 4, "OVP Limit");
	}
	viCheckParm( Ivi_SetAttributeViBoolean (vi, channelName, 
	                                    CPX400_ATTR_OVP_ENABLED,
	                                    0, enabled), 3, "OVP Enabled");
    
	checkErr( CPX400_CheckStatus (vi));

Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_ConfigureOCP                                   
 * Purpose:  This function enables or disables over-current protection and
 *           configures the over-current limit.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ConfigureOCP (ViSession vi,
                                         ViConstString channelName,
                                         ViReal64 limit)
{
	ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
	                                 CPX400_ATTR_OCP_LIMIT,
	                                 0, limit), 4, "OCP Limit");
	checkErr( CPX400_CheckStatus (vi));

Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: CPX400_ConfigureVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ConfigureVoltageLevel (ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 level)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	
	
	
	error = Ivi_SetAttributeViReal64 (vi, channelName, CPX400_ATTR_VOLTAGE_LEVEL,
                                   IVI_VAL_DIRECT_USER_CALL, level);

	
	
	Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;
}

/*****************************************************************************
 * Function: CPX400_QueryMaxCurrentLimit                  
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
ViStatus _VI_FUNC CPX400_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64* maxCurrentLimit)
{
  ViStatus error = VI_SUCCESS;
  ViReal64 value = 0;
	ViChar rdBuffer[BUFFER_SIZE];
  
  checkErr( Ivi_LockSession(vi, VI_NULL));

  if (maxCurrentLimit == VI_NULL)
          viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxCurrentLimit.");
  
  if (Ivi_RangeChecking(vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
  	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      	
 	  switch (PSUModel)
		{
			case CPX400DP:
				value = 20.0;
			default:
				value = 20.0;
				break;
		}
  }
	
  checkErr( CPX400_CheckStatus (vi));

Error:    
  if (maxCurrentLimit) *maxCurrentLimit = value;
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: CPX400_QueryMaxVoltageLevel                  
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
ViStatus _VI_FUNC CPX400_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64* maxVoltageLevel)
{
  ViStatus error = VI_SUCCESS;
  ViReal64 value = 0;
	ViChar rdBuffer[BUFFER_SIZE];
  
  checkErr( Ivi_LockSession(vi, VI_NULL));

  if (maxVoltageLevel == VI_NULL)
          viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxVoltageLevel.");
  
  if (Ivi_RangeChecking(vi))
  {
 //   viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),2, "Channel Name");
	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      	
 	  switch (PSUModel)
		{
			case CPX400DP: 	
				value = 60.0;
				break;
			
			default:
				value = 60.0;
				break;
		}
	}
	
  checkErr( CPX400_CheckStatus (vi));

Error:    
  if (maxVoltageLevel) *maxVoltageLevel = value;
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
}

/*****************************************************************************
 * Function: CPX400_ResetOutputProtection                                   
 * Purpose:  This function resets the power supply's output protection.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ResetOutputProtection (ViSession vi, ViConstString channelName)
{
  ViStatus error = VI_SUCCESS;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
  
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  
    viCheckErr( viPrintf (io, "TRIPRST\n"));
  }
  checkErr( CPX400_CheckStatus (vi));
  
Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
          
}

/*****************************************************************************
 * Function: CPX400_IncrementVoltageLevel
 * Purpose: Increments the output voltage by the step size set for the
 *					output channel specified.
 *
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_IncrementVoltageLevel (ViSession vi,
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
    if (VerifyVoltage)
      viCheckErr( viPrintf (io, "INCV%sV\n", channelName));
    else
      viCheckErr( viPrintf (io, "INCV%s\n", channelName));
  }

  checkErr( CPX400_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: CPX400_DecrementVoltageLevel
 * Purpose: Decrements the output voltage by the step size set for the
 *					output channel specified. 
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_DecrementVoltageLevel (ViSession vi,
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
    if (VerifyVoltage)
    	viCheckErr( viPrintf (io, "DECV%sV\n", channelName));
    else
    	viCheckErr( viPrintf (io, "DECV%s\n", channelName));
  }

  checkErr( CPX400_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: CPX400_IncrementCurrentLevel
 * Purpose:  Increments the current level limit by the step size set for the
 *						output channel specified.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_IncrementCurrentLevel (ViSession vi,
                                              ViChar channelName[])
{
  ViStatus error = VI_SUCCESS;
  ViAttr   attrId;
  
  checkErr( Ivi_LockSession(vi, VI_NULL));
  
  if (Ivi_RangeChecking (vi))
  {
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL),  2, "Channel Name");
  	  viCheckParm( Ivi_CoerceChannelName (vi, channelName, &channelName), 2, "Channel");      
  }

  if (!Ivi_Simulating (vi))
  {
    ViSession io = Ivi_IOSession(vi);
      
    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    viCheckErr( viPrintf (io, "INCI%s\n", channelName));
  }

  checkErr( CPX400_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: CPX400_DecrementCurrentLevel
 * Purpose: Decrements the current level limit by the step size set for the
 *					output channel specified.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_DecrementCurrentLevel (ViSession vi,
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

  checkErr( CPX400_CheckStatus (vi));

Error:    
  Ivi_UnlockSession(vi, VI_NULL);
  return error;
            
}

/*****************************************************************************
 * Function: CPX400_Measure                                   
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
	{CPX400_VAL_MEASURE_CURRENT, 0, 0, "I", CPX400_ATTR_CURRENT_LIMIT},
	{CPX400_VAL_MEASURE_VOLTAGE, 0, 0, "V", CPX400_ATTR_VOLTAGE_LEVEL},
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

ViStatus _VI_FUNC CPX400_Measure (ViSession vi,
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
//    viCheckParm( Ivi_CoerceChannelName (vi, channelName, VI_NULL), 2, "Channel Name");
	
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


  checkErr( CPX400_CheckStatus (vi));

Error:    
  if (measurement) *measurement = value;
  	Ivi_UnlockSession(vi, VI_NULL);
  return error;
          
}

/*****************************************************************************
 * Function: CPX400_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC CPX400_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: CPX400_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC CPX400_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: CPX400_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the CPX400_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC CPX400_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC CPX400_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC CPX400_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/*****************************************************************************
 * Function: CPX400_GetNextCoercionRecord                        
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
ViStatus _VI_FUNC CPX400_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/*****************************************************************************
 * Function: CPX400_LockSession and CPX400_UnlockSession Functions                        
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
ViStatus _VI_FUNC CPX400_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC CPX400_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/*****************************************************************************
 * Function: CPX400_GetErrorInfo and CPX400_ClearErrorInfo Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC CPX400_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}
ViStatus _VI_FUNC CPX400_GetError (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;                                  
}                                                                                                           
ViStatus _VI_FUNC CPX400_ClearError (ViSession vi)                                                        
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
ViStatus _VI_FUNC CPX400_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC CPX400_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 


/*------------[NEW]----------------------------------------------------------*/
/*****************************************************************************
 * Function: CPX400_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
*****************************************************************************/
ViStatus _VI_FUNC CPX400_InvalidateAllAttributes (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_InvalidateAllAttributes(vi);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;    
}

/*****************************************************************************
 * Function: CPX400_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( CPX400_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}
/**************************************************************************** 
 *  Function: CPX400_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC CPX400_Disable (ViSession vi)
{
     ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

					 
	error = Ivi_SetAttributeViBoolean (vi, VI_NULL, CPX400_ATTR_OUTPUT_ENABLED_ALL,
                                   IVI_VAL_DIRECT_USER_CALL, VI_FALSE);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;

}

/**************************************************************************** 
 *  Function: CPX400_GetNextInterchangeWarning,
 *            CPX400_ResetInterchangeCheck, and
 *            CPX400_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC CPX400_GetNextInterchangeWarning (ViSession vi, 
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
 * Function: CPX400_ResetInterchangeCheck                                       
 * Purpose:                                       
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ResetInterchangeCheck (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_ResetInterchangeCheck (vi);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;    
}

/*****************************************************************************
 * Function: CPX400_ClearInterchangeWarnings                                       
 * Purpose:                                       
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ClearInterchangeWarnings (ViSession vi)
{
    ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	error = Ivi_ClearInterchangeWarnings (vi);
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;    
}


/*****************************************************************************
 * Function: CPX400_ConfigureOutputRange                                       
 * Purpose:  Configures the power supply's output range on a channel.                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_ConfigureOutputRange (ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 rangeType,
                                                 ViReal64 range)
{
  ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));

	if (rangeType == CPX400_VAL_RANGE_CURRENT || rangeType == CPX400_VAL_RANGE_VOLTAGE)
	{	
		error = Ivi_SetAttributeViInt32 (vi, channelName, CPX400_ATTR_RANGE_SETTING,
	                                   IVI_VAL_DIRECT_USER_CALL, range);
	}
Error:    
	Ivi_UnlockSession(vi, VI_NULL);
	
	return error;

            
}
/*****************************************************************************
 * Function: CPX400_QueryOutputState                                       
 * Purpose:  This function returns whether the power supply is in a
 *					 particular output state.                                     
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
ViStatus _VI_FUNC CPX400_QueryOutputState (ViSession vi,
                                             ViConstString channelName,
                                             ViInt32 outputState,
                                             ViBoolean* inState)
{
   ViStatus error = VI_SUCCESS;

	checkErr( Ivi_LockSession (vi, VI_NULL));
	
	
	switch (outputState)
	{
		case CPX400_VAL_OUTPUT_CONSTANT_VOLTAGE:
		case CPX400_VAL_OUTPUT_CONSTANT_CURRENT:
		case CPX400_VAL_OUTPUT_OVER_VOLTAGE:
		case CPX400_VAL_OUTPUT_OVER_CURRENT:    
		case CPX400_VAL_OUTPUT_UNREGULATED:     
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

    checkErr( CPX400_CheckStatus (vi));

Error:    
    if (inState) *inState = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
*/            
}

/**************************************************************************** 
 *  Function: CPX400_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC CPX400_GetChannelName (ViSession vi, ViInt32 index,
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
 *  Function: CPX400_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option.  The driver setup string
 *            is part of the option string the user passes to the
 *            CPX400_InitWithOptions function.  Specifically it is the part
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
static ViStatus CPX400_GetDriverSetupOption (ViSession vi,
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
 * Function: CPX400_CheckStatus                                                 
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
static ViStatus CPX400_CheckStatus (ViSession vi)
{
  ViStatus    error = VI_SUCCESS;

  if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
  {
	  checkErr( CPX400_CheckStatusCallback (vi, Ivi_IOSession(vi)));
	  checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
  }
      
Error:
    return error;
}

/*****************************************************************************
 * Function: CPX400_WaitForOPC                                                  
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
static ViStatus CPX400_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
  ViStatus    error = VI_SUCCESS;

  if (!Ivi_Simulating(vi))
  {
	  ViInt32  oldOPCTimeout; 
  
	  checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CPX400_ATTR_OPC_TIMEOUT, 
	                                     0, &oldOPCTimeout));
	  Ivi_SetAttributeViInt32 (vi, VI_NULL, CPX400_ATTR_OPC_TIMEOUT,        
	                           0, maxTime);

	  error = CPX400_WaitForOPCCallback (vi, Ivi_IOSession(vi));

	  Ivi_SetAttributeViInt32 (vi, VI_NULL, CPX400_ATTR_OPC_TIMEOUT, 
	                           0, oldOPCTimeout);
	  viCheckErr( error);
  }
Error:
  return error;
}

/*****************************************************************************
 * Function: CPX400_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           CPX400_reset function calls this function.  The 
 *           CPX400_IviInit function calls this function when the
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
static ViStatus CPX400_DefaultInstrSetup (ViSession vi)
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
static ViStatus CPX400_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus CPX400_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: CPX400_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           CPX400_CheckStatus utility function.  The 
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
static ViStatus _VI_FUNC CPX400_CheckStatusCallback (ViSession vi, ViSession io)
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
		//CPX400_query_error_query( vi, &Ret, ErrMsg);
	  viCheckErr( IVI_ERROR_INSTR_SPECIFIC);
  }
          
Error:
  return error;
}

/*****************************************************************************
 * Function: CPX400_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           CPX400_WaitForOPC utility function.  The IVI engine invokes
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
static ViStatus _VI_FUNC CPX400_WaitForOPCCallback (ViSession vi, ViSession io)
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

  checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CPX400_ATTR_OPC_TIMEOUT, 
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

/*- CPX400_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC CPX400AttrIdQueryResponse_ReadCallback (ViSession vi, 
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
    
/*- CPX400_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC CPX400AttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      driverRevision[256];
  
  
  sprintf (driverRevision, 
           "Driver: CPX400 %i.%i, Compiler: %s %3.2f, "
           "Components: IVIEngine %.2f, VISA-Spec %.2f",
           CPX400_MAJOR_VERSION, CPX400_MINOR_VERSION, 
           IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
           IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
           Ivi_ConvertVISAVer(VI_SPEC_VERSION));

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
  return error;
}


/*- CPX400_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC CPX400AttrFirmwareRevision_ReadCallback (ViSession vi, 
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

/*- CPX400_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC CPX400AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      rdBuffer[BUFFER_SIZE];
  ViChar      idQ[BUFFER_SIZE];
  
  checkErr( Ivi_GetAttributeViString (vi, "", CPX400_ATTR_ID_QUERY_RESPONSE,
                                      0, BUFFER_SIZE, idQ));
  sscanf (idQ, "%256[^,]", rdBuffer);

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
  
Error:
  return error;
}

    

/*- CPX400_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC CPX400AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
  ViStatus    error = VI_SUCCESS;
  ViChar      rdBuffer[BUFFER_SIZE];
  ViChar      idQ[BUFFER_SIZE];
  
  checkErr( Ivi_GetAttributeViString (vi, "", CPX400_ATTR_ID_QUERY_RESPONSE,
                                      0, BUFFER_SIZE, idQ));
  sscanf (idQ, "%*[^,],%256[^,]", rdBuffer);

  checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
  
Error:
  return error;
}
    

/*- CPX400_ATTR_OUTPUT_ENABLED -*/

static ViStatus _VI_FUNC CPX400AttrOutputEnabled_WriteCallback (ViSession vi, 
                                                                 ViSession io,
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "OP%s %hd\n", channelName, value ? 1 : 0));
  
Error:
  return error;
}


/*- CPX400_ATTR_VOLTAGE_LEVEL -*/
static IviRangeTableEntry attrVoltageLevelRangeTableEntries[] =
	{
		{0.0, 60, 0, "0", CPX400_VAL_RANGE_1},
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
    
static ViStatus _VI_FUNC CPX400AttrVoltageLevel_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  

  if (VerifyVoltage)
  	viCheckErr( viPrintf (io, "V%sV %lf\n", channelName, value));
  else
  	viCheckErr( viPrintf (io, "V%s %lf\n", channelName, value));

Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrVoltageLevel_ReadCallback (ViSession vi, 
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

/*- CPX400_ATTR_OVP_ENABLED -*/

/*- CPX400_ATTR_OVP_LIMIT -*/
static IviRangeTableEntry attrOVPLimitRangeTableEntries[] =
	{
		{0, 66, 0, "", 0},
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

static ViStatus _VI_FUNC CPX400AttrOVPLimit_WriteCallback (ViSession vi, 
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

static ViStatus _VI_FUNC CPX400AttrOVPLimit_ReadCallback (ViSession vi, 
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

/*- CPX400_ATTR_CURRENT_LIMIT_BEHAVIOR -*/

/*=PTP KCS:===============================================================*

  NOTE TO THE DEVELOPER: As the PSU only supports regulate the following 
  changes have been made:
  
  - Delete the CPX400AttrCurrentLimitBehavior_ReadCallback
    and CPX400AttrCurrentLimitBehavior_WriteCallback
    function prototypes and definitions.
  
  - Pass the instrument's fixed value as the default value 
    parameter of the Ivi_AddAttribute function that creates this attribute.

  - Modify the attrCurrentLimitBehaviorRangeTable range table so that
    the only valid value is the current limit behavior your instrument
    supports.
  
*============================================================END=PTP KCS*/

static IviRangeTableEntry attrCurrentLimitBehaviorRangeTableEntries[] =
{
	{CPX400_VAL_CURRENT_TRIP, 0, 0, "TRP", 0},
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
    


/*- CPX400_ATTR_CURRENT_LIMIT -*/
static IviRangeTableEntry attrCurrentLimitRangeTableEntries[] =
	{
		{0.0, 20, 0, "", CPX400_VAL_RANGE_1},
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

static ViStatus _VI_FUNC CPX400AttrCurrentLimit_WriteCallback (ViSession vi, 
                                                                ViSession io,
                                                                ViConstString channelName, 
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
  ViStatus error = VI_SUCCESS;

  viCheckErr( viPrintf (io, "I%s %lf\n", channelName, value));
  
  
Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrCurrentLimit_ReadCallback (ViSession vi, 
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
																		

static ViStatus _VI_FUNC CPX400AttrRangeSetting_WriteCallback (ViSession vi,
                                                           ViSession io,
                                                           ViConstString channelName,
                                                           ViAttr attributeId,
                                                           ViInt32 value)
{
  char Cmd[100];
	ViStatus error = VI_SUCCESS;


  
Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrRangeSetting_ReadCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 *value)
{
  char Cmd[10], CmdRet[100];
  ViStatus error = VI_SUCCESS;
  


Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrOCPLimit_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrOCPLimit_WriteCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViReal64 value){
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "OCP%s %lf\n", channelName, value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrIflock_ReadCallback (ViSession vi,
                                                        ViSession io,
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

static ViStatus _VI_FUNC CPX400AttrIflock_WriteCallback (ViSession vi,
                                                         ViSession io,
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

static ViStatus _VI_FUNC CPX400AttrVoltageStepSize_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrVoltageStepSize_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "DELTAV%s %lf\n", channelName, value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrCurrentStepSize_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrCurrentStepSize_WriteCallback (ViSession vi,
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



static ViStatus _VI_FUNC CPX400AttrLimitStatusRegister1_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister1_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister1_WriteCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrStandardEventEnableRegister_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrStandardEventEnableRegister_WriteCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrIstLocalMessage_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrStandardEventStatusRegister_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrOperationCompleteStatus_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrOperationCompleteStatus_WriteCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrParallelPoleEnableRegister_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrParallelPoleEnableRegister_WriteCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrServiceRequestEnableRegister_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrServiceRequestEnableRegister_WriteCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrStatusByteRegister_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrAddress_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViInt32 *value)
{
  ViStatus error = VI_SUCCESS;
  
  //viCheckErr( viQueryf (io, "CONFIG?\n", "%ld", value));
  viCheckErr( viQueryf (io, "ADDRESS?\n", "%ld", value));

Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrVerifyVoltage_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrVerifyVoltage_WriteCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrOutputEnabledAll_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
  ViStatus error = VI_SUCCESS;
	ViInt32 Channels;
  
	//findout which model we are
	CPX400_GetAttributeViInt32 (vi, "", CPX400_ATTR_NUM_CHANNELS, &Channels);
	if (Channels < 2)
		viCheckErr(IVI_ERROR_FUNCTION_NOT_SUPPORTED);
  
  viCheckErr( viPrintf (io, "OPALL %hd\n", value ? 1 : 0));
  
Error:
  return error;
}


static ViStatus _VI_FUNC CPX400AttrLimitStatusRegister2_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister2_ReadCallback (ViSession vi,
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

static ViStatus _VI_FUNC CPX400AttrLimitStatusEnableRegister2_WriteCallback (ViSession vi,
                                                                           ViSession io,
                                                                           ViConstString channelName,
                                                                           ViAttr attributeId,
                                                                           ViInt32 value){
  ViStatus error = VI_SUCCESS;
  
  viCheckErr( viPrintf (io, "LSE%s %ld\n", "2", value));
  
Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrTrackingMode_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value)
{
	char Ret[2];
	
	ViStatus	error = VI_SUCCESS;
	
  viCheckErr( viQueryf (io, "CONFIG?\n", "%s", Ret));
	switch (Ret[0])
	{
			case '0':
				*value = attrConfigRangeTableEntries[0].discreteOrMinValue;
				break;
			case '2':
				*value = attrConfigRangeTableEntries[1].discreteOrMinValue;
				break;
	}

Error:
	return error;
}




static ViStatus _VI_FUNC CPX400AttrRatio_ReadCallback (ViSession vi,
                                                       ViSession io,
                                                       ViConstString channelName,
                                                       ViAttr attributeId,
                                                       ViReal64 *value)
{
  char Cmd[10];
  ViStatus error = VI_SUCCESS;
  
	sprintf ( Cmd, "RATIO?\n");
  viCheckErr( viQueryf (io, Cmd, "%lf", value));
  

Error:
  return error;
}

static ViStatus _VI_FUNC CPX400AttrRatio_WriteCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 value)
{
  ViStatus error = VI_SUCCESS;

  viCheckErr( viPrintf (io, "RATIO %lf\n", value));
  
  
Error:
  return error;
}


static ViStatus _VI_FUNC CPX400AttrTrackingMode_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	
	ViInt32		trackingMode;
	ViBoolean	enabled;
	
	char Ret[2];
	
	viCheckErr( viQueryf (io, "CONFIG?\n", "%s", Ret));  
	trackingMode = atoi(Ret);
	

//	checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CPX400_ATTR_TRACKING_MODE, 0,&trackingMode));
	
	error = Ivi_GetAttributeViBoolean (vi, "2", CPX400_ATTR_OUTPUT_ENABLED,
                                      IVI_VAL_DIRECT_USER_CALL, &enabled);
	
	
	if (enabled && (trackingMode == 2) && (value == 0))
		  viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must turn output 2 off to enter tracking mode.");
	
	

	if (enabled && (trackingMode == 0) && (value == 2))
		  viCheckErrElab( IVI_ERROR_INVALID_CONFIGURATION,
               "Must turn output 2 off to exit tracking mode.");
	
	viCheckErr( viPrintf (io, "CONFIG %d\n", value));   

Error:
	return error;
}

/*****************************************************************************
 * Function: CPX400_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *
 *	Copyright (c) 2008 Product Technology Partners
 *
 *	Author	Kevin Snelling
 *
 *****************************************************************************/
static ViStatus CPX400_InitAttributes (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;
	ViInt32     flags = 0;
	    /*=PTP KCS:=============================================================*

	        NOTE TO THE DEVELOPER:  You can add additional parameters to the
	        prototype of this function.  This is useful when you want to pass
	        information from the initialization functions.  The Attribute Editor 
	        in LabWindows/CVI requires that the name of this function be 
	        CPX400_InitAttributes.

	     *==========================================================END=PTP KCS=*/

	    /*- Initialize instrument attributes --------------------------------*/            

	checkErr( Ivi_SetAttributeViInt32 (vi, "", CPX400_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
	                                   0, CPX400_MAJOR_VERSION));
	checkErr( Ivi_SetAttributeViInt32 (vi, "", CPX400_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
	                                   0, CPX400_MINOR_VERSION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CPX400_ATTR_SPECIFIC_DRIVER_REVISION,
	                                           CPX400AttrDriverRevision_ReadCallback));
	checkErr( Ivi_SetAttributeViInt32 (vi, "", CPX400_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
	                                   0, CPX400_CLASS_SPEC_MAJOR_VERSION));
	checkErr( Ivi_SetAttributeViInt32 (vi, "", CPX400_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
	                                   0, CPX400_CLASS_SPEC_MINOR_VERSION));
	checkErr( Ivi_SetAttributeViString (vi, "", CPX400_ATTR_IO_SESSION_TYPE,
	                                    0, CPX400_IO_SESSION_TYPE));
	checkErr( Ivi_SetAttributeViString (vi, "", CPX400_ATTR_SUPPORTED_INSTRUMENT_MODELS,
	                                    0, CPX400_SUPPORTED_INSTRUMENT_MODELS));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CPX400_ATTR_INSTRUMENT_FIRMWARE_REVISION,
	                                           CPX400AttrFirmwareRevision_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, CPX400_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CPX400_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
	                               flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_GetAttributeFlags (vi, CPX400_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CPX400_ATTR_INSTRUMENT_MANUFACTURER, 
	                               flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CPX400_ATTR_INSTRUMENT_MANUFACTURER,
	                                         CPX400AttrInstrumentManufacturer_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, CPX400_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CPX400_ATTR_INSTRUMENT_MODEL, 
	                               flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CPX400_ATTR_INSTRUMENT_MODEL,
	                                         CPX400AttrInstrumentModel_ReadCallback));
	checkErr( Ivi_SetAttributeViString (vi, "", CPX400_ATTR_SPECIFIC_DRIVER_VENDOR,
	                                    0, CPX400_DRIVER_VENDOR));
	checkErr( Ivi_SetAttributeViString (vi, "", CPX400_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
	                                    0, CPX400_DRIVER_DESCRIPTION));
	checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
	                                  CPX400_WaitForOPCCallback));
	checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
	                                  CPX400_CheckStatusCallback));
	checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE, 
	                                   0, VI_FALSE));
	checkErr( Ivi_SetAttributeViString (vi, "", CPX400_ATTR_GROUP_CAPABILITIES, 0,
	                                  "IviDCPwrBase,"
	                                  "IviDCPwrTrigger,"
	                                  "IviDCPwrSoftwareTrigger,"
	                                  "IviDCPwrMeasurement"));

	    /*- Add instrument-specific attributes ------------------------------*/            
	checkErr (Ivi_AddAttributeViBoolean (vi, CPX400_ATTR_OUTPUT_ENABLED,
	                                     "CPX400_ATTR_OUTPUT_ENABLED", VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL, VI_NULL,
	                                     CPX400AttrOutputEnabled_WriteCallback));
  
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_VOLTAGE_LEVEL,
	                                    "CPX400_ATTR_VOLTAGE_LEVEL", 1.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrVoltageLevel_ReadCallback,
	                                    CPX400AttrVoltageLevel_WriteCallback,
	                                    &attrVoltageLevelRangeTable, 0));
                                       
	checkErr (Ivi_AddAttributeViBoolean (vi, CPX400_ATTR_OVP_ENABLED,
	                                     "CPX400_ATTR_OVP_ENABLED", VI_TRUE,
	                                     IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL));
  
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_OVP_LIMIT,
	                                    "CPX400_ATTR_OVP_LIMIT", 66,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrOVPLimit_ReadCallback,
	                                    CPX400AttrOVPLimit_WriteCallback,
	                                    &attrOVPLimitRangeTable, 0));
  
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_CURRENT_LIMIT_BEHAVIOR,
	                                 "CPX400_ATTR_CURRENT_LIMIT_BEHAVIOR",
	                                 CPX400_VAL_CURRENT_TRIP,
	                                 IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL,
	                                 &attrCurrentLimitBehaviorRangeTable));
  
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_CURRENT_LIMIT,
	                                    "CPX400_ATTR_CURRENT_LIMIT", 1.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrCurrentLimit_ReadCallback,
	                                    CPX400AttrCurrentLimit_WriteCallback,
	                                    &attrCurrentLimitRangeTable, 0));
  
	checkErr (Ivi_AddAttributeViString (vi, CPX400_ATTR_ID_QUERY_RESPONSE,
	                                    "CPX400_ATTR_ID_QUERY_RESPONSE",
	                                    "THURLBY THANDAR, CPX400DP",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    CPX400AttrIdQueryResponse_ReadCallback,
	                                    VI_NULL));
                                         
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_OPC_TIMEOUT,
	                                   "CPX400_ATTR_OPC_TIMEOUT", 5000,
	                                   IVI_VAL_DONT_CHECK_STATUS | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrOpcTimeoutRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_RANGE_SETTING,
	                                   "CPX400_ATTR_RANGE_SETTING", 0,
	                                   IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                   CPX400AttrRangeSetting_ReadCallback,
	                                   CPX400AttrRangeSetting_WriteCallback,
	                                   &attrRangeSettingRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_OCP_LIMIT,
	                                    "CPX400_ATTR_OCP_LIMIT", 21,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrOCPLimit_ReadCallback,
	                                    CPX400AttrOCPLimit_WriteCallback,
	                                    &attrOCPLimitRangeTable, 0));
	checkErr (Ivi_AddAttributeViBoolean (vi, CPX400_ATTR_IFLOCK, "CPX400_ATTR_IFLOCK",
	                                     VI_FALSE, 0, CPX400AttrIflock_ReadCallback,
	                                     CPX400AttrIflock_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_VOLTAGE_STEP_SIZE,
	                                    "CPX400_ATTR_VOLTAGE_STEP_SIZE", 0.01,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrVoltageStepSize_ReadCallback,
	                                    CPX400AttrVoltageStepSize_WriteCallback,
	                                    &attrVoltageLevelRangeTable, 0));
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_CURRENT_STEP_SIZE,
	                                    "CPX400_ATTR_CURRENT_STEP_SIZE", 0.01,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrCurrentStepSize_ReadCallback,
	                                    CPX400AttrCurrentStepSize_WriteCallback,
	                                    &attrCurrentLimitRangeTable, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_LIMIT_STATUS_REGISTER_1,
	                                   "CPX400_ATTR_LIMIT_STATUS_REGISTER_1", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_MULTI_CHANNEL,
	                                   CPX400AttrLimitStatusRegister1_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_LIMIT_STATUS_ENABLE_REGISTER_1,
	                                   "CPX400_ATTR_LIMIT_STATUS_ENABLE_REGISTER_1", 0,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   CPX400AttrLimitStatusEnableRegister1_ReadCallback,
	                                   CPX400AttrLimitStatusEnableRegister1_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_STANDARD_EVENT_ENABLE_REGISTER,
	                                   "CPX400_ATTR_STANDARD_EVENT_ENABLE_REGISTER", 0,
	                                   0,
	                                   CPX400AttrStandardEventEnableRegister_ReadCallback,
	                                   CPX400AttrStandardEventEnableRegister_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi,
	                                   CPX400_ATTR_STANDARD_EVENT_STATUS_REGISTER,
	                                   "CPX400_ATTR_STANDARD_EVENT_STATUS_REGISTER",
	                                   0, IVI_VAL_NOT_USER_WRITABLE,
	                                   CPX400AttrStandardEventStatusRegister_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_IST_LOCAL_MESSAGE,
	                                   "CPX400_ATTR_IST_LOCAL_MESSAGE", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   CPX400AttrIstLocalMessage_ReadCallback, VI_NULL,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_OPERATION_COMPLETE_STATUS,
	                                   "CPX400_ATTR_OPERATION_COMPLETE_STATUS", 0, 0,
	                                   CPX400AttrOperationCompleteStatus_ReadCallback,
	                                   CPX400AttrOperationCompleteStatus_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_PARALLEL_POLE_ENABLE_REGISTER,
	                                   "CPX400_ATTR_PARALLEL_POLE_ENABLE_REGISTER", 0,
	                                   0,
	                                   CPX400AttrParallelPoleEnableRegister_ReadCallback,
	                                   CPX400AttrParallelPoleEnableRegister_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_SERVICE_REQUEST_ENABLE_REGISTER,
	                                   "CPX400_ATTR_SERVICE_REQUEST_ENABLE_REGISTER",
	                                   0, 0,
	                                   CPX400AttrServiceRequestEnableRegister_ReadCallback,
	                                   CPX400AttrServiceRequestEnableRegister_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_STATUS_BYTE_REGISTER,
	                                   "CPX400_ATTR_STATUS_BYTE_REGISTER", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   CPX400AttrStatusByteRegister_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_ADDRESS, "CPX400_ATTR_ADDRESS", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE,
	                                   CPX400AttrAddress_ReadCallback, VI_NULL,
	                                   &attrAddressRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, CPX400_ATTR_VERIFY_VOLTAGE,
	                                     "CPX400_ATTR_VERIFY_VOLTAGE", VI_FALSE,
	                                     IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
	                                     CPX400AttrVerifyVoltage_ReadCallback,
	                                     CPX400AttrVerifyVoltage_WriteCallback));
	checkErr (Ivi_AddAttributeViBoolean (vi, CPX400_ATTR_OUTPUT_ENABLED_ALL,
	                                     "CPX400_ATTR_OUTPUT_ENABLED_ALL", VI_FALSE,
	                                     0, VI_NULL,
	                                     CPX400AttrOutputEnabledAll_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_LIMIT_STATUS_REGISTER_2,
	                                   "CPX400_ATTR_LIMIT_STATUS_REGISTER_2", 0,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_MULTI_CHANNEL,
	                                   CPX400AttrLimitStatusRegister2_ReadCallback,
	                                   VI_NULL, &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_LIMIT_STATUS_ENABLE_REGISTER_2,
	                                   "CPX400_ATTR_LIMIT_STATUS_ENABLE_REGISTER_2", 0,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   CPX400AttrLimitStatusEnableRegister2_ReadCallback,
	                                   CPX400AttrLimitStatusEnableRegister2_WriteCallback,
	                                   &attrStatusRegisterRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CPX400_ATTR_TRACKING_MODE,
	                                   "CPX400_ATTR_TRACKING_MODE", 2,
	                                   IVI_VAL_NEVER_CACHE,
	                                   CPX400AttrTrackingMode_ReadCallback,
	                                   CPX400AttrTrackingMode_WriteCallback,
	                                   &attrConfigRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CPX400_ATTR_RATIO, "CPX400_ATTR_RATIO",
	                                    100, IVI_VAL_MULTI_CHANNEL,
	                                    CPX400AttrRatio_ReadCallback,
	                                    CPX400AttrRatio_WriteCallback,
	                                    &attrRatioRangeTable, 0));
                                   
	/*- Setup attribute invalidations -----------------------------------*/            

	checkErr (Ivi_AddAttributeInvalidation (vi, CPX400_ATTR_VERIFY_VOLTAGE,
	                                      CPX400_ATTR_VOLTAGE_LEVEL, VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, CPX400_ATTR_RANGE_SETTING,
	                                        CPX400_ATTR_VOLTAGE_LEVEL, VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, CPX400_ATTR_RANGE_SETTING,
	                                        CPX400_ATTR_CURRENT_LIMIT, VI_TRUE));
	Error:
	return error;    
}

static ViStatus CPX400_InitRangeTables (ViSession vi)
{
	ViStatus    error = VI_SUCCESS;
	ViChar rdBuffer[BUFFER_SIZE];

	//customise the range table to the specific model, note that the default is the PL303QMD-P
	checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CPX400_ATTR_ID_QUERY_RESPONSE, 0, BUFFER_SIZE, rdBuffer));

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

