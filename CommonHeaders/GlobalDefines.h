#ifndef __GlobalDefines_H__
#define __GlobalDefines_H__

//==============================================================================
// Include files
#include <windows.h> 
#include "cvidef.h"

//==============================================================================
// Global Includes

//#define	__TE_PROFILING__
//==============================================================================
// Constants

#define				 DEFAULT_DEBUG_PATH 						"C:\\ATE\\WorkSpace"

//======================= Mudeles Global ===========================//
#define				STD_STRING									1024
#define				LOW_STRING									128 

#define				EMPTYPARAM									0
#define				LASTPARAM									-1L 

#define				STD_BUFFER_SIZE								1024
#define				STD_DOUBLE_BUFFER_SIZE						2048  
//==================== Registy ======================================//

#define				ETS_REG_SUB_KEY_MODULE_FORMATED				"Software\\ATE\\%s"

#define				VISA_OPEN_LOCK								"Visa_Open_Lock"
//==============================================================================
// Types
#define		EQUIPMENT_TYPE_STANDARD_DEVICE						1
#define		EQUIPMENT_TYPE_POWER_SUPPLY							2
#define		EQUIPMENT_TYPE_NETWORK_ANALYZER						3
#define		EQUIPMENT_TYPE_SIGNAL_GENERATOR						4
#define		EQUIPMENT_TYPE_SPECTRUM_ANALYZER					5
#define		EQUIPMENT_TYPE_OSCILLOSCOPE							6
#define		EQUIPMENT_TYPE_FUNCTION_GENERATOR					7
#define		EQUIPMENT_TYPE_POWER_METER							8
#define		EQUIPMENT_TYPE_NOISE_FIGURE							9
#define		EQUIPMENT_TYPE_MULTI_METER							10
#define		EQUIPMENT_TYPE_WAVE_FORM_ANALYZER					11

//-------------- Standard device api use keys ---------------//
#define		STANDARD_PROTOCOL_DRIVER_DEVICE_ID					0x0A0000001
#define		STANDARD_OVEN_CONTROL_MODBUS_DEVICE_ID				0x0A0000002
#define		STANDARD_DAQMX_DRIVER_DEVICE_ID						0x0A0000003
#define		STANDARD_FTDI_DRIVER_DEVICE_ID						0x0A0000004
#define		STANDARD_COMMUNICATION_LOW_LEVEL_DEVICE_ID			0x0A0000005
#define		STANDARD_LAB_CONTROL_DEVICE_ID						0x0A0000006
#define		STANDARD_CLI_DRIVER_DEVICE_ID						0x0A0000007
#define		STANDARD_ICS_DRIVER_DEVICE_ID						0x0A0000008
#define		STANDARD_MINI_CIRCUITS_RFBOX_DEVICE_ID				0x0A0000009
#define		STANDARD_RIO_DRIVER_DEVICE_ID						0x0A000000A
#define		STANDARD_EXTENDED_PROTOCOL_DRIVER_DEVICE_ID			0x0A000000B

//==============================================================================
// Error Defines
typedef char tStandardString[STD_STRING];
typedef char tLowLengthString[LOW_STRING]; 

typedef struct
{
	union
	{
		char									max_size[128];   
		
		struct					
		{
			void								*pNextLevelError; 
	
			int									error;

			unsigned char						*pszErrorDescription,
												*pszErrorFrom,
												*pszFile,
												*pszFunction;

			int									line,
												threadID;
		};
	};
	
} STD_ERROR;

typedef void (*pfCommentCallback)( int iMessageType ,  char *pszText, void *pData );
typedef void (*pfConfigValueCallback)( unsigned long long ullDataBaseLinkID , unsigned long long ullEquipmentHandle , int iMultiIndex , char *szModuleName , char *szValueName , int ValueType , int iValueSize , void *pValue , void *pDefaultValue , void *pData );  
typedef int	 (*pfCheckForBreakCallback)( void *pData ); 
typedef int	 (*pfFileCallback)( int iSection , char* pszFileName , char** pszFileFullPath , void *pData ); 

typedef struct
{
	union
	{
		char							max_size[STD_BUFFER_SIZE];   
		
		struct					
		{
			char						szName[LOW_STRING];
	
			pfCommentCallback			fCommentCallback;
			void						*pCommentCallbackData;
			int							commentType;
	
			pfConfigValueCallback		fConfigValueCallback;
			void						*pConfigValueCallbackData; 
			int							configType;
	
			pfCheckForBreakCallback		fCheckForBreakCallback;
			void						*pCheckForBreakCallbackData; 
			int							breakType;
	
			int							hThreadID;
	
			int							iMultiUsingIndex;
			
			int							bDisableLog;

			int							iInternalHandle,
										iEquipmentHandle;
										
			unsigned long long			ullDataBaseLinkID;
			
			int							bDemoMode;
			
			char						szAddress[LOW_STRING];
			
			//------------- 15.05.2014 ---------------------------------//
			pfFileCallback				fFileCallback;
			void						*pFileCallbackData;  
			int							iFileType;
			
		};
	};
	
} tsSTD_CallBackSet;


typedef union
{
	char				buffer[8],
						sByte;
	unsigned char		usByte;
	unsigned short		uWord;
	short				sWord;
	unsigned int		udWord;
	int					sdWord;
	float				fWord;
	unsigned __int64	uqWord;
	__int64				sqWord;
	double				lfWord;
	
} tuConvert;

typedef union
{
	char				max_size[STD_BUFFER_SIZE];   
	
	struct
	{   
		unsigned char	sendHeader,
						opCode;
		
		unsigned short	dataSize;
	
		long long		iSendValue,
						iResponseValue;
		
		unsigned char	responseHeader, 
						responseOpCode;
		
		unsigned short	responseDataSize;
		
		char			szCommandName[128];
		
		//------------ Signature --------------------------------//
		unsigned __int64			ulSignatureID;
		//-------------------------------------------------------//	
		
		int				sendingSizeType,
						responseSizeType;
						
		unsigned int	sendReceiveDelay_Msec;
	};
	
} tsSTD_CommandItem;


typedef union
{
	char				max_size[STD_BUFFER_SIZE];   
	
	struct
	{   
		unsigned short	sendHeader,
						opCode;
		
		unsigned int	dataSize;
	
		long long		iSendValue,
						iResponseValue;
		
		unsigned short	responseHeader, 
						responseOpCode;
		
		unsigned int	responseDataSize;
		
		unsigned int	sendingSizeType,
						responseSizeType;
		
		char			szCommandName[128];
		
		//------------ Signature --------------------------------//
		unsigned __int64			ulSignatureID;
		//-------------------------------------------------------//	
		
		unsigned int	sendReceiveDelay_Msec; 
	};
	
} tsSTD_Extended_CommandItem;

typedef void* (*tfStdReceiveCallback) ( void *pHandle , char *pCommand , int *pValue , int iValueSize , char *pReceiveBuffer , int iBufferSize );

//--------------------------------------------- memory macros --------------------------------------------------------------//
#define		STRING_CONVERT(xConstant)  																		#xConstant 

#define		FREE(xPointer)																					{ if ( xPointer ) { free(xPointer); xPointer = NULL; }}	

#define		FREE_LIST_ONLY(xPointer,xCount)																	{ int xxIndex=0; if ( xPointer ) { if ( xCount ) { for(xxIndex=0;xxIndex<xCount;xxIndex++) { FREE(xPointer[xxIndex]);}}}} 
#define		FREE_LIST(xPointer,xCount)																		{ FREE_LIST_ONLY(xPointer,xCount); free(xPointer); xPointer = NULL; }

#define		CALLOC(xyPointer,xyCount,xySizeof)																{ int xxCount = xyCount; void *xxpData = NULL; if (xySizeof==1) xxCount++; if ((xxCount>0) && (xySizeof>0)) { xxpData = calloc(xxCount,xySizeof); xyPointer = xxpData; }}
#define		CALLOC_CONST_SINGLE(xyPointer,xySizeof)															{ void *xxpData = NULL; xxpData = calloc(1,xySizeof); xyPointer = xxpData; }   

#define		CALLOC_ERR(xPointer,xCount,xSizeof)																{ CALLOC(xPointer,xCount,xSizeof); IF ((xPointer==NULL),"Can't allocate memory.");}
#define		CALLOC_CONST_SINGLE_ERR(xPointer,xSizeof)														{ CALLOC_CONST_SINGLE(xPointer,xSizeof); IF ((xPointer==NULL),"Can't allocate memory.");}

#define		FREE_CALLOC_ERR(xPointer,xCount,xSizeof)														{ FREE(xPointer); CALLOC_ERR(xPointer,xCount,xSizeof);}
#define		FREE_CALLOC(xPointer,xCount,xSizeof)															{ FREE(xPointer); CALLOC(xPointer,xCount,xSizeof); }   

#define		FREE_CALLOC_CONST_SINGLE(xPointer,xSizeof)														{ FREE(xPointer); CALLOC_CONST_SINGLE(xPointer,xSizeof); }
#define		FREE_CALLOC_CONST_SINGLE_ERR(xPointer,xSizeof)													{ FREE(xPointer); CALLOC_CONST_SINGLE_ERR(xPointer,xSizeof);}

#define		CALLOC_COPY(xPointer,xCount,xSizeof,xFromPointer,xCopySize)										{ int iNewCCCCC = xCount; if (xCopySize>(iNewCCCCC*(xSizeof))) {iNewCCCCC = (((xCopySize)/(xSizeof))+2);} CALLOC(xPointer,iNewCCCCC,xSizeof); if (xCopySize>0) { if (xFromPointer&&xPointer){ memcpy(xPointer,xFromPointer,xCopySize);}else {FREE(xPointer);}}}   
#define		CALLOC_COPY_ERR(xPointer,xCount,xSizeof,xFromPointer,xCopySize)									{ CALLOC_ERR(xPointer,xCount,xSizeof); IF (((xFromPointer==NULL) || (xCopySize>((xCount)*(xSizeof)))),"Can't copy memory."); if (xCopySize>0) memcpy(xPointer,xFromPointer,xCopySize);}

#define		CALLOC_COPY_CONST_SINGLE(xPointer,xFromPointer,xSizeof)											{ CALLOC_CONST_SINGLE(xPointer,xSizeof); if ( xFromPointer && xPointer ){ memcpy(xPointer,xFromPointer,xSizeof);}else {FREE(xPointer);}}   
#define		CALLOC_COPY_CONST_SINGLE_ERR(xPointer,xFromPointer,xSizeof)										{ CALLOC_CONST_SINGLE_ERR(xPointer,xSizeof); if ( xFromPointer && xPointer ){ memcpy(xPointer,xFromPointer,xSizeof);}else {FREE(xPointer);}}   

#define		FREE_CALLOC_COPY(xPointer,xCount,xSizeof,xFromPointer,xCopySize)								{ FREE_CALLOC(xPointer,xCount,xSizeof); if (xCopySize>0) { if (xFromPointer&&xPointer&&(xCopySize<=((xCount)*(xSizeof)))) memcpy(xPointer,xFromPointer,xCopySize);else FREE(xPointer)}}      
#define		FREE_CALLOC_COPY_ERR(xPointer,xCount,xSizeof,xFromPointer,xCopySize)							{ FREE_CALLOC_ERR(xPointer,xCount,xSizeof); IF (((xFromPointer==NULL) || (xCopySize>((xCount)*(xSizeof)))),"Can't copy memory."); if (xCopySize>0) memcpy(xPointer,xFromPointer,xCopySize);}    

#define		CALLOC_COPY_STRING(xPointer,xFromPointer)														{ if ( xFromPointer ) { CALLOC_COPY(xPointer,(strlen(xFromPointer)+1),sizeof(char),xFromPointer,strlen(xFromPointer)) }}
#define		FREE_CALLOC_COPY_STRING(xPointer,xFromPointer)													{ if ( xFromPointer ) { FREE_CALLOC_COPY(xPointer,(strlen(xFromPointer)+1),sizeof(char),xFromPointer,strlen(xFromPointer)) }}

#define		CALLOC_COPY_STRING_ERR(xPointer,xFromPointer)													{ if ( xFromPointer ) { CALLOC_COPY_ERR(xPointer,(strlen(xFromPointer)+1),sizeof(char),xFromPointer,strlen(xFromPointer)) }}
#define		FREE_CALLOC_COPY_STRING_ERR(xPointer,xFromPointer)												{ if ( xFromPointer ) { FREE_CALLOC_COPY_ERR(xPointer,(strlen(xFromPointer)+1),sizeof(char),xFromPointer,strlen(xFromPointer)) }}

//--------- Use after "Error:" -----------------------------//
#define		STDERR_COPY(xPointer)																			{ STD_ERROR *xxFcStdError = (STD_ERROR*)xPointer; if ( xxFcStdError ) { memcpy( &StdError , xxFcStdError , sizeof(STD_ERROR)); COPY_STD_ERROR((&StdError),xxFcStdError); }}
#define		FREE_STDERR_COPY(xPointer)																		{ STD_ERROR *xxFccStdError = (STD_ERROR*)xPointer; STDERR_COPY(xxFccStdError); FREE(xxFccStdError);}  
#define		FREE_STDERR_COPY_ERR(xPointer)																	{ STD_ERROR *xxFccStdError = (STD_ERROR*)xPointer; STDERR_COPY(xxFccStdError); FREE(xxFccStdError); CHK_STDERR(StdError); }


#define		FREE_STDERR_POINTER(xxPointer)																	{ STD_ERROR	*xxFStdError=NULL,*xxFStdPrevError=NULL,*xxRefStdError=(STD_ERROR*)xxPointer; while ( xxRefStdError ) { FREE(xxRefStdError->pszErrorDescription); FREE(xxRefStdError->pszErrorFrom); FREE(xxRefStdError->pszFile); FREE(xxRefStdError->pszFunction); if ( xxRefStdError->pNextLevelError ) { xxFStdError = (STD_ERROR*)xxRefStdError->pNextLevelError; if ( xxFStdPrevError == xxFStdError ) break; xxRefStdError->pNextLevelError = NULL; memcpy(xxRefStdError , xxFStdError , sizeof(STD_ERROR)); if ( xxFStdError ) xxFStdPrevError = xxFStdError; FREE(xxFStdError); } else { break;}}}
#define		FREE_STDERR																						{ FREE_STDERR_POINTER((&StdError)); }

#define		RETURN_STDERR_POINTER																			{ STD_ERROR *xxRStdError=NULL; CALLOC_COPY_CONST_SINGLE(xxRStdError,&StdError,sizeof(STD_ERROR));  return xxRStdError;}
#define		RETURN_STDERR																					{ return StdError;}
#define		RETURN_ERROR_VALUE																				{ FREE_STDERR; return StdError.error;}
#define		RETURN_VALUE(xxRetValue)																		{ FREE_STDERR; return xxRetValue;}
#define		RETURN_VOID																						{ FREE_STDERR; return;}
//----------------------------------------------------------//
//--------------------------------------------- memory macros --------------------------------------------------------------//


#define			MODULE_NAME																					__FILE__
#define			STDF_UPDATE_CALLBACK_DATA(xSTD_CallBackSet)													{ if ( xSTD_CallBackSet ) memcpy((&tSTD_CallBackSet),xSTD_CallBackSet,1024);}

//Example to use  STDF_COMMENT(0,"Configuration of UUT %s communication channel.","N%d"); 
#define			STDF_COMMENT(xx_MessageType,yy_Text_With_FormatedS,zz_Formated)								{ if ( tSTD_CallBackSet.fCommentCallback && ( tSTD_CallBackSet.bDisableLog == 0 )) { char ss_String[STD_STRING],ff_Foramted[STD_STRING]={"\b"}; if(tSTD_CallBackSet.iMultiUsingIndex){sprintf(ff_Foramted,zz_Formated,tSTD_CallBackSet.iMultiUsingIndex); sprintf(ss_String,yy_Text_With_FormatedS,ff_Foramted);} else {sprintf(ss_String,yy_Text_With_FormatedS,"");} tSTD_CallBackSet.fCommentCallback(xx_MessageType,ss_String,tSTD_CallBackSet.pCommentCallbackData);} }

#define			STDF_CONFIG_VALUE(nn_ValueName,xx_ValueType,yy_ValueSize,zzVoidValue,ttDefaultValue)		{ if ( tSTD_CallBackSet.fConfigValueCallback ) { tSTD_CallBackSet.fConfigValueCallback( tSTD_CallBackSet.ullDataBaseLinkID , tSTD_CallBackSet.iEquipmentHandle , tSTD_CallBackSet.iMultiUsingIndex,tSTD_CallBackSet.szName,nn_ValueName,xx_ValueType,yy_ValueSize,((void*)&zzVoidValue),((void*)&ttDefaultValue),tSTD_CallBackSet.pConfigValueCallbackData);}}
#define			STDF_CHECK_BREAK_PAUSE																		{ if ( tSTD_CallBackSet.fCheckForBreakCallback ) { if ( tSTD_CallBackSet.fCheckForBreakCallback(tSTD_CallBackSet.pCheckForBreakCallbackData)) { FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) "Break for upper layer" ); goto Error;}} }
#define			STDF_GET_MULTY_USING																		tSTD_CallBackSet.iMultiUsingIndex
#define			STDF_SET_MODULE_NAME(xSTD_CallBackSet,xxName)												{ if ( xSTD_CallBackSet && xxName && ( strlen(xxName))){ strcpy( xSTD_CallBackSet->szName , xxName );}}
#define			STDF_GET_EQUIPMENT_STATE_FILE(xxStateName,yyStateFileFullPath)								{ if ( xxStateName && strlen(xxStateName) && tSTD_CallBackSet.fFileCallback ) { tSTD_CallBackSet.fFileCallback( tSTD_CallBackSet.iFileType , xxStateName , &yyStateFileFullPath , tSTD_CallBackSet.pFileCallbackData);} }

typedef enum
{
	ERROR_NO = ERROR_SUCCESS,					// ERROR_SUCCESS
	ERROR_REPORTED = -1,							// 
	ERROR_PASSED_NULL = -2,   						// ERROR_CLUSTER_NULL_DATA
	ERROR_ALLOCATION_MEMORY = -3,					// ERROR_OUTOFMEMORY
	ERROR_FUNCTION_INPUT_OUT_OF_RANGE = -4,			// ERROR_CLUSTER_PARAMETER_OUT_OF_BOUNDS
	ERROR_CHECK_EXPRESSION_FAIL = -5,				// ERROR_DS_COMPARE_FALSE
	ERROR_INDEX_OVERFLOW = -6,						// DISP_E_BADINDEX
	ERROR_TIMEOUT_IS_OVER = -7,
	
	ERROR_SENTINEL	
}	gteErrorsList;

#define				NUMBER_OF_ERRORS				((ERROR_SENTINEL*-1)+1)
extern const char	errorsDescriptions[NUMBER_OF_ERRORS][1024];																																			

// Macros Defines
// in used function define "StdError" variable by type (STD_ERROR)  
	
#define		FREE_FILL_ERROR_POSITION																		{ FREE_CALLOC_COPY_STRING( (StdError.pszFile) , __FILE__ ); FREE_CALLOC_COPY_STRING( (StdError.pszFunction) , __FUNCTION__ ); StdError.line = __LINE__ ; StdError.threadID = CmtGetCurrentThreadID();}


#define		FREE_FILL_ERROR_POSITION_XXX																	{ FREE_CALLOC_COPY_STRING( tempxxxxx.pszFile , __FILE__ ); FREE_CALLOC_COPY_STRING( tempxxxxx.pszFunction , __FUNCTION__ ); tempxxxxx.line = __LINE__ ; }  
#define		START_DEBUG_PRINT(xDescription)																	{ GetCurrentDateTime(&startTimeXXXYYY); sprintf(debug__print__string,  "Started                    ===>   %s   <======          \n" , xDescription ); OutputDebugString(debug__print__string);}
#define		STOP_DEBUG_PRINT(xDescription)																	{ GetCurrentDateTime(&currentTimeXXXYYY); sprintf(debug__print__string,  "Finish Function [%0.3lf] {{ 0x0%X }}||  %s || in func \"%s\" line %d file %s\n" , (currentTimeXXXYYY - startTimeXXXYYY) , CurrThreadId() , xDescription , tempxxxxx.pszFunction , tempxxxxx.line , tempxxxxx.pszFile );OutputDebugString(debug__print__string);}
	
#define		IF(xError,xDescription)																			{ StdError.error = xError; if (StdError.error) { if (StdError.error==1) StdError.error = ERROR_CHECK_EXPRESSION_FAIL; FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom), MODULE_NAME ); FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) , xDescription );  FREE_FILL_ERROR_POSITION; goto Error;}} 
#define		IF_BREAK(xError,xDescription)																	{ StdError.error = xError; if (StdError.error) { if (StdError.error==1) StdError.error = ERROR_CHECK_EXPRESSION_FAIL; FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom), MODULE_NAME ); FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) , xDescription );  FREE_FILL_ERROR_POSITION; break;}} 
#define		IF_CONTINUE(xError,xDescription)																{ StdError.error = xError; if (StdError.error) { if (StdError.error==1) StdError.error = ERROR_CHECK_EXPRESSION_FAIL; FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom), MODULE_NAME ); FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) , xDescription );  FREE_FILL_ERROR_POSITION; continue;}} 

#define		IF_END(xError,xDescription)																			{ StdError.error = xError; if (StdError.error) { if (StdError.error==1) StdError.error = ERROR_CHECK_EXPRESSION_FAIL; FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom), MODULE_NAME ); FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) , xDescription );  FREE_FILL_ERROR_POSITION;}} 

#define		CHK_CMT(xError)																					{ StdError.error = xError; if (StdError.error<0) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"Multithreading CVI"); CmtGetErrorMessage ( StdError.error ,  (StdError.pszErrorDescription) ); FREE_FILL_ERROR_POSITION; goto Error;}} 

#define		CHK_UIL(xError)																					{ StdError.error = xError; if (StdError.error<0) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"CVI User Interface Library."); FREE_CALLOC_COPY_STRING( (StdError.pszErrorDescription) , GetUILErrorString ( StdError.error )); FREE_FILL_ERROR_POSITION; goto Error;}} 

#define		CHK_SQL(xError)																					{ StdError.error = xError; if (!SQL_SUCCEEDED(StdError.error)) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"SQL Data Base."); FREE_CALLOC_COPY_STRING( (StdError.pszErrorDescription) , "SQL Data Base Error."); goto Error;}}
				
#define		CHK_VSA(xError)																					{ StdError.error = xError; if (StdError.error<0) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"VISA CVI"); FREE_FILL_ERROR_POSITION; goto Error;}} 
#define		CHK_VSA_ERROR																					{ if (StdError.error<0) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"VISA CVI"); CALLOC(StdError.pszErrorDescription,STD_STRING,sizeof(char)); if ( StdError.pszErrorDescription ) viStatusDesc ( visaObjHandle , StdError.error ,  (StdError.pszErrorDescription) ); FREE_FILL_ERROR_POSITION; goto Error;}} 

#define		CHK_DAQMX(xError)																				{ StdError.error = xError; if (StdError.error) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"DAQmx CVI"); FREE_CALLOC_ERR((StdError.pszErrorDescription) , STD_STRING , sizeof(char)); DAQmxGetErrorString ( StdError.error ,  StdError.pszErrorDescription , STD_STRING ); FREE_FILL_ERROR_POSITION; goto Error;}} 

#define		CHK_FTDI(xError)																				{ StdError.error = xError; if (StdError.error) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom) , "FTDI Error" ); FTDI_GetErrorMessage ( StdError.error , (StdError.pszErrorDescription) ); goto Error;}}   

#define		CHK_INI(xError)																					{ StdError.error = xError; if (StdError.error<0) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"IniFile Instrument"); FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) , "Ini file function error." ); FREE_FILL_ERROR_POSITION; goto Error;}}
#define		CHK_INI0(xError)																				{ StdError.error = xError; if (StdError.error<=0) { FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),"IniFile Instrument"); FREE_CALLOC_COPY_STRING((StdError.pszErrorDescription) , "Ini file function error." ); FREE_FILL_ERROR_POSITION; goto Error;}} 

#define		CLEAR_STD_ERROR																					{ FREE_STDERR; memset( &StdError , 0 , sizeof(STD_ERROR)); }	
#define		FILL_STDERR(xError)																				{ STD_ERROR *xxFillStdError = (STD_ERROR*)xError; FREE_STDERR; memcpy( &StdError , xxFillStdError , sizeof(STD_ERROR)); COPY_STD_ERROR( (&StdError) , xxFillStdError );}	
#define		GET_STDERR_CLEAR(xError)																		{ STD_ERROR *xxGetStdError = NULL; int xxyError = StdError.error; CALLOC( xxGetStdError , 1 , sizeof(STD_ERROR)); if ( xxGetStdError ) { memcpy( xxGetStdError , &StdError , sizeof(STD_ERROR)); memset( (&StdError) , 0 , sizeof(STD_ERROR)); FREE_STDERR_POINTER(xError); xError = xxGetStdError; StdError.error = xxyError;}}	

#define		CHK_STDERR(xError)																				{ StdError = xError; if (StdError.error<0) { GET_STDERR_CLEAR((StdError.pNextLevelError)); FREE_FILL_ERROR_POSITION; goto Error;}else{ FREE_STDERR; }}  
#define		CHK_STD_ERROR																					{ if (StdError.error<0) { GET_STDERR_CLEAR((StdError.pNextLevelError)); FREE_FILL_ERROR_POSITION; goto Error;}}  

#define		NO_CHK_STDERR(xError)																			{ STD_ERROR xxInputStdError = xError; FREE_STDERR_POINTER(&xxInputStdError); }

#define		STDERR(xError)																					{ StdError.error = xError; }  
#define		UPDATERR(xError)																				{ STD_ERROR *xxUpStdError = NULL; FREE_STDERR; StdError = xError; if ( StdError.error ) { GET_STDERR_CLEAR(xxUpStdError); FREE_FILL_ERROR_POSITION; StdError.pNextLevelError = xxUpStdError; }else{FREE_STDERR;}}  

#define		IS_OK																							(StdError.error == ERROR_SUCCESS)
#define		IS_NOT_OK																						(StdError.error != ERROR_SUCCESS)

#define		COPY_STD_ERROR(xxxTo,xxxFrom)																	{ CALLOC_COPY_STRING((xxxTo->pszErrorFrom), (xxxFrom->pszErrorFrom) ); CALLOC_COPY_STRING((xxxTo->pszErrorDescription) , (xxxFrom->pszErrorDescription) ); CALLOC_COPY_STRING((xxxTo->pszFile),(xxxFrom->pszFile)); CALLOC_COPY_STRING((xxxTo->pszFunction),(xxxFrom->pszFunction)); };

//--- Debug macros ----//
//#define		CHK_StdError(xError)																		{ STD_ERROR		tempxxxxx; double startTimeXXXYYY = 0.0,currentTimeXXXYYY = 0.0; char debug__print__string[1024]={0}; FREE_FILL_ERROR_POSITION_XXX; START_DEBUG_PRINT(#xError); StdError = xError; STOP_DEBUG_PRINT(#xError); if (StdError.error<0) goto Error;}
//#define		CHK_CMT(xError)																				{ STD_ERROR		tempxxxxx; double startTimeXXXYYY = 0.0,currentTimeXXXYYY = 0.0; char debug__print__string[1024]={0}; FREE_FILL_ERROR_POSITION_XXX; START_DEBUG_PRINT(#xError); StdError.error = xError; STOP_DEBUG_PRINT(#xError); if (StdError.error<0) { FREE_CALLOC_COPY_STRING(StdError.pszErrorFrom,"Multithreading CVI"); CmtGetErrorMessage ( StdError.error ,  StdError.pszErrorDescription ); FREE_FILL_ERROR_POSITION; goto Error;}} 


#define		CHK_STDERR_BREAK(xError)																		{ StdError = xError; if (StdError.error<0) break;}  
#define		CHK_STDERR_CONTINUE(xError)																		{ StdError = xError; if (StdError.error<0) continue;}  

#define		SET_ERR(xError,xDescription)																	{ StdError.error = xError; FREE_CALLOC_COPY_STRING((StdError.pszErrorFrom),MODULE_NAME); FREE_CALLOC_COPY_STRING ((StdError.pszErrorDescription) , xDescription ); FREE_FILL_ERROR_POSITION;}

#define		SET_DESCRIPTION(xDescription)																	{ if ( xDescription ) { FREE_CALLOC_COPY_STRING ((StdError.pszErrorDescription), xDescription ); FREE_FILL_ERROR_POSITION;}}

#define		FORCE_ERR(xError,xDescription)																	{ SET_ERR(xError,xDescription); goto Error; } 

#define		STD_ERR(xError)																					{ StdError.error = xError; if (StdError.error<0) { FREE_FILL_ERROR_POSITION; goto Error; }}
#define		CHK_ERR(xError)																					{ StdError.error = xError; if (StdError.error) { FREE_FILL_ERROR_POSITION; goto Error; }}  

#define		CHK_ERR_BREAK(xError)																			{ StdError.error = xError; if (StdError.error) { FREE_FILL_ERROR_POSITION; break; }} 
#define		CHK_ERR_CONTINUE(xError)																		{ StdError.error = xError; if (StdError.error) { FREE_FILL_ERROR_POSITION; continue; }} 

#define		CHK_STDERR_BREAK(xError)																		{ StdError = xError; if (StdError.error) { GET_STDERR_CLEAR((StdError.pNextLevelError)); FREE_FILL_ERROR_POSITION; break; }} 
#define		CHK_STDERR_CONTINUE(xError)																		{ StdError = xError; if (StdError.error) { GET_STDERR_CLEAR((StdError.pNextLevelError)); FREE_FILL_ERROR_POSITION; continue; }} 

#define		DELAY(x)																						{ double	xxxStartTime = 0.0, xxxCurrentTime = 0.0;  GetCurrentDateTime( &xxxStartTime ); do { GetCurrentDateTime( &xxxCurrentTime ); ProcessSystemEvents(); }while(( xxxCurrentTime - xxxStartTime ) < x );}

//--------- Use before "Error:" -----------------------------//
#define		RETURN																							{ goto Error;} 
#define		RETURN_ERR(xError)																				{ StdError.error = xError; FREE_FILL_ERROR_POSITION; goto Error;}  
//----------------------------------------------------------//

#define		SET																								1
#define		GET																								0
#define		CLEAR																							0
#define		ENABLE																							1
#define		DISABLE																							0
		

//==============================================================================
// Global functions

//void CVICALLBACK MSG_Messenger(void *Data);

#endif  /* ndef __GlobalDefines_H__ */
