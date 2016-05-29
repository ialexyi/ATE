/*======= Master signal generator driver code - Version 03.11 ======*/
/* For mi2023, mi2024, mi3002, ri3271, ifr2023A, ifr2023B, ifr2025 */
/* Produced by
    IFR Ltd.
    Longacres House,
    Norton Green Road,
    Stevenage,
    Herts SG1 2BA
    Phone +44-438-742200
    Fax   +44-438-727601

    V2.02   12/03 New parameters to ifr2025_sweepQuery(),sweepMode() and sweep()
                  New array for LIN/LOG string constant
                  New functions for carrier limit and offset; new constants

    V2.03   12/06/96  Add calls to set FLUSH on viScanf, buffer sizes and END on
                      last byte. All in ifr2025_init.
                      Fix RFoffset - command was not constructed correctly.
    
    V2.04   05/08/96  Modified sweep query to accomodate updates to mi2024.
    
    V2.05   03/12/96  Added initialisation for Serial Communications when used for 2024
    
    V2.06	03/07/98  Added new variants 2023A, 2023B & 2025 (IFR)
                      Includes different max freq limits and Sinad option

    V2.07	14/08/98  DBMV read back for sfp amended.
    
    V2.08	17/09/98  Add delta level step to ifr2025_sinad and ifr2025_sinadSetupQuery functions
                      Add new function ifr2025_sinadReadyQuery to query new Sinad status bit
                      
    V3.00	09/10/98  Track SFP issue                  

	V3.01	24/02/99  Version change only following patch to distribution kit script                  

	V3.10	22/03/99  Version change only following correction to funtion panel

    V3.11	15/09/99  Added code to capture variations in Racal *IDN? response

    V3.20	01/05/03  DLL now supports Win2k
                      Removed ifr2025_installAppHandler ifr2025_uninstallAppHandler
                          ifr2025_serviceSRQ to allow DLL to linked to .fp
						  ifr2025_applicationHandler handler removed form sgData
						  This fixes problem with use in LabView/HP Vee
					  Corrected bugs in ifr2025_setFSK, ifr2025_memPowerUp	
					  Some of the Windows names have been changed in .FP
					    file to clear duplicate erros in LabView/HP Vee
					    NOTE: this has NOT effected the function call names
*/
/*=========================================================================*/

#include "toolbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ansi_c.h> /* 3.11 */

#include "visa.h"
#include "vpptype.h"
#include "ifr2025.h"


ViStatus checkStatus( ViSession vi, ViStatus error );

#define DO_DEBUG

#ifdef DO_DEBUG
  #define RETURN( vi, err ) return( checkStatus( vi, err ) )
#else
  #define RETURN( vi, err ) return( err )
#endif

#define CHECKVAL( val, min, max, err ) \
     ( (val < min) || (val > max) ? err : VI_SUCCESS ) 

/* 
  This structure is used to hold per-session data. This allows things like
  RF Type to be stored in VISA for inspection by routines that need this value
  specified.
*/
struct tagsgData
{
  ViInt32   null1;
  ViInt16   rfType;                   /* ifr2025_RFTYPE_EMF or ifr2025_RFTYPE_PD */
  ViInt16   instrument;               /* mi3002, mi2023, mi2024 etc */
  ViBoolean highPower;                /* VI_TRUE if 25dBm > 1.2GHz */
  ViBoolean sinad;                    /* VI_TRUE if Sinad option fitted */
  ViUInt16  checkMode;                /* check on routine exit ? */
  ViUInt16  stb;                      /* copy of device STB on routine exit */
  ViUInt16  errorCount;
  ViReal64  version;                  /* instrument firmware version */
  
};
typedef struct tagsgData sgData;

/*=========================================================================*/
/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS ===================================*/
/*=========================================================================*/
static long timeout[18] =
  { -1, 1, 1, 1, 1, 1, 3, 10, 30, 100,
    300, 1000, 3000, 10000, 30000,100000, 300000, 1000000 };

static ViReal64 pdMin[7] =
  { -137.0, -150.0, -90.0, -30.0, 31.6e-9, 31.6e-6, 31.6e-3 };
static ViReal64 pdMax[7] =
  { 25.1,     12.0,  72.0, 132.0,  4.0,    4.0e3,     4.0e6 };
  /* dBm     dBV    dBmV    dBuV    V       mV         uV   */

static ViReal64 emfMin[7] =
  { -137.0, -144.0, -84.0, -24.0, 63.2e-9, 63.2e-6, 63.2e-3 };
static ViReal64  emfMax[7] =
  { 25.1,    18.1,  78.1,   138.1,  8.0,   8.0e3,    8.0e6  };
  /* dBm     dBV    dBmV    dBuV    V       mV         uV   */

static char *units[7] = { "DBM", "DBV", "DBMV", "DBUV", "V", "MV", "UV" };
static char *state[2] = { "OFF", "ON" };;
static char *atten_tab[2] = {"UNLOCK", "LOCK" };
static char *rf_out[2] = { "DISABLE", "ENABLE" };
static char *rfType[2] = { "EMF", "PD" };
static char *mod_mode_tab[7] = {"AM", "FM", "PM", "AM,FM", "AM,PM", "FSK2L", "FSK4L" };
static char *mod_channel[9] = { "AM", "AM1", "AM2", "FM", "FM1", "FM2", "PM", "PM1", "PM2" };
static char *pulse[2] = { "", ",PULSE"};
static char *mod_source[4] = {"INT", "EXTAC", "EXTALC", "EXTDC" };
static char *mod_waveform[3] = { "SIN", "TRI", "SQR"};
static char *cfrq_mode_tab[2] = {"SWEPT", "FIXED" };
static char *sweep_mode_tab[2] = { "CONT", "SNGL" };
static char *sweep_trig_tab[4] = { "OFF", "START", "STARTSTOP", "STEP" };
static char *sweep_control_tab[8] = {"GO", "HALT", "CONT", "RESET", "RETN", "XFER", "UP", "DN" };
static char *trig_src_tab[11] = {"HOLD", "TTLTRG0", "TTLTRG1", "TTLTRG2", "TTLTRG3", "TTLTRG4", "TTLTRG5", "TTLTRG6", "TTLTRG7","BUS", "EXTERNAL" };
static char *mem[4] = {"CFRQ", "FULL", "RAM", "ALL" };
static int mem_limit[3] = { 99, 199, 299 };
static char *seq_mode[10] = {"OFF", "SEQ1", "SEQ2", "SEQ3", "SEQ4", "SEQ5", "SEQ6", "SEQ7", "SEQ8", "SEQ9"};
static char *mem_dir[2] = { "DN", "UP" };
static char *fstd_type_tab[5] = { "INT",  "EXT10DIR", "EXT1IND", "EXT10IND", "INT10OUT"};
static char *powup[2] = {"FACTORY", "MEMORY" };
static char *sweepType[2] = { "LIN", "LOG" };

ViStatus checkStatus( ViSession vi, ViStatus error )
{
  ViStatus  status;
  ViUInt16  stb;
  sgData    *pM;

  status = viGetAttribute( vi, VI_ATTR_USER_DATA, &pM );
  if( status >= VI_SUCCESS )
  {
    if( pM != NULL )
    {
      if( pM->checkMode == ifr2025_CHECK_ON_EXIT )
      {
        status = viReadSTB( vi, &stb );
        if( status >= VI_SUCCESS )
        {
          pM->stb = stb;
          if( stb & 0xFF )
            pM->errorCount++;
        }
      }
    }
  }
  return error;
}

/* ================ Error messages ============= */
struct tagmiErr
{
  ViStatus  error;
  ViString  msg;
};
typedef struct tagmiErr miErr;

static miErr miErrors[] =
{
  { ifr2025_ERROR_AM2_LIM_AM1,   "AM2 limited by AM1" },
  { ifr2025_ERROR_INST_NOT_SUPP, "Instrument function not supported" },
  { ifr2025_ERROR_RFUNITS,       "Illegal RF Units specified" },
  { ifr2025_ERROR_RFLEVEL,       "RF level out of range" },
  { ifr2025_ERROR_ONOFF,         "Must specify ON or OFF only" },
  { ifr2025_ERROR_RFTYPE,        "RF Type must be EMF or PD" },
  { ifr2025_ERROR_RFFREQ,        "RF frequency out of range" },
  { ifr2025_ERROR_PHASE,         "Phase out of range" },
  { ifr2025_ERROR_MODMODE,       "Illegal modulation mode" },
  { ifr2025_ERROR_PULSE,         "Illegal pulse mode selection" },
  { ifr2025_ERROR_SOURCE,        "Illegal source selection" },
  { ifr2025_ERROR_RATE,          "Modulation rate out of range" },
  { ifr2025_ERROR_DEPTH,         "Modulation depth/deviation out of range" },
  { ifr2025_ERROR_WAVEFORM,      "Illegal waveform" },
  { ifr2025_ERROR_STATE,         "State should be ON or OFF" },
  { ifr2025_ERROR_MODCHAN,       "Modulation channel should be 1 or 2" },
  { ifr2025_ERROR_STARTFREQ,     "Start frequency out of range" },
  { ifr2025_ERROR_STOPFREQ,      "Stop frequency out of range" },
  { ifr2025_ERROR_STEPFREQ,      "Step frequency out of range" },
  { ifr2025_ERROR_STEPTIME,      "Step time out of range" },
  { ifr2025_ERROR_DDM_TYPE,      "Invalid DDM type specified" },
  { ifr2025_ERROR_BEARING,       "Bearing out of range" },
  { ifr2025_ERROR_RFOFFSET,      "RF level offset out of range" },
  { ifr2025_ERROR_SINADAV,       "Sinad averages out of range" },
  { VI_ERROR_PARAMETER1,        "Parameter 1 invalid or out of range" },
  { VI_ERROR_PARAMETER2,        "Parameter 2 invalid or out of range" },
  { VI_ERROR_PARAMETER3,        "Parameter 3 invalid or out of range" },
  { VI_ERROR_PARAMETER4,        "Parameter 4 invalid or out of range" },
  { VI_ERROR_PARAMETER5,        "Parameter 5 invalid or out of range" },
  { VI_ERROR_PARAMETER6,        "Parameter 6 invalid or out of range" },
  { VI_ERROR_PARAMETER7,        "Parameter 7 invalid or out of range" },
  { VI_ERROR_PARAMETER8,        "Parameter 8 invalid or out of range" },
  { VI_SUCCESS, "No error" }
} ;

/*=========================================================================*/
/* Function: initialiseSerialCom                                           */
/* Purpose:                                                                */
/*  Initialise the serial communications port associated with this session.*/
/*  This routine is called from ifr2025_init if the resource is "ASRL..."   */
/*=========================================================================*/
ViStatus initialiseSerialCom( ViSession vi )
{
  ViStatus  status;
  
    status = viSetAttribute( vi, VI_ATTR_ASRL_BAUD, 9600 );

  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_ASRL_DATA_BITS , 8 );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_ASRL_PARITY , VI_ASRL_PAR_NONE );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_ASRL_STOP_BITS , VI_ASRL_STOP_ONE );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_ASRL_FLOW_CNTRL , VI_ASRL_FLOW_XON_XOFF );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_ASRL_END_IN , VI_ASRL_END_TERMCHAR );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_ASRL_END_OUT , VI_ASRL_END_NONE );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_WR_BUF_OPER_MODE , VI_FLUSH_ON_ACCESS );
  
  if( status >= VI_SUCCESS )
    status = viSetAttribute( vi, VI_ATTR_RD_BUF_OPER_MODE , VI_FLUSH_ON_ACCESS );
  
  return( status );
}

/*=========================================================================*/
/* Function: Initialize                                                    */
/* Purpose:  This function opens the instrument, queries the instrument    */
/*           for its ID, and initializes the instrument to a known state.  */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_init (ViRsrc resource, ViBoolean idQuery, ViBoolean doReset, ViPSession vi)
{
  ViSession defRM;
  ViStatus  status = VI_SUCCESS;
  sgData    *pM = NULL;
  ViPChar	pManuf, pModel;
  char      *s; /* 3.11 String pointer for uppercase conversion */

  *vi = VI_NULL;

  status = viOpenDefaultRM (&defRM);
  if( status >= VI_SUCCESS )
    status = viOpen( defRM, resource, VI_NULL, VI_NULL, vi );

    /* now initialise serial comms if the interface is ASRL (serial) */
  if( status >= VI_SUCCESS )
  {
    ViAttr  type;
    
    viGetAttribute (*vi, VI_ATTR_INTF_TYPE, (ViAddr) &type);
    if( type == VI_INTF_ASRL )
      status = initialiseSerialCom( *vi );
  }
  
  if( status >= VI_SUCCESS )
  {
    pM = (sgData *) malloc( sizeof(sgData) );
    if( pM == NULL )
      status = VI_ERROR_ALLOC;
    else
    {
      status = viSetAttribute( *vi, VI_ATTR_USER_DATA, (ViAttrState) pM );
      pM->checkMode = ifr2025_CHECK_ON_EXIT;
      pM->stb = 0;
      pM->errorCount = 0;
    }
  }

  if( status >= VI_SUCCESS )
    status = viSetAttribute (*vi, VI_ATTR_TMO_VALUE, 2000);
    /* Send END on the last byte of transfer */ /* V2.03 */

  if( status >= VI_SUCCESS )
    status = viSetAttribute(*vi, VI_ATTR_SEND_END_EN, (ViAttrState)VI_TRUE);

    /* Set the input/output buffers to 256 characters */ /* V2.03 */
  if( status >= VI_SUCCESS )
    status = viSetBuf (*vi, VI_READ_BUF|VI_WRITE_BUF, (ViUInt32)256);

    /* Make sure the input buffer flushes after every viScanf call */ /* V2.03 */
  if( status >= VI_SUCCESS )
    status = viSetAttribute(*vi, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);


  /*  Initialize the instrument to a known state.  */
  if( status >= VI_SUCCESS )
    if( doReset == VI_TRUE)
      status = ifr2025_reset(*vi);


  if( status >= VI_SUCCESS )
  {
    if( idQuery == VI_TRUE)
    {
      ViChar  idString[256];

      status = viPrintf(*vi, "*IDN?\n" );
      if( status >= VI_SUCCESS )
      {
        status = viScanf(*vi, "%t", idString );
        if( status >= VI_SUCCESS )
        {
          pManuf = strtok( idString, "," );  /* MARCONI INSTRUMENTS */

          /* 3.11 Convert manufacturer string to uppercase */
          
          for ( s = pManuf; *s; ++s )
          	*s = toupper(*s);

          if( strcmp( pManuf, "MARCONI INSTRUMENTS" ) == 0 )
          {
            pModel = strtok( NULL, "," );      /* 3002 */
            if( strcmp( pModel, "3002" ) == 0 )
              pM->instrument = mi3002;
            else if( strcmp( pModel, "2023" ) == 0 )
              pM->instrument = mi2023;
            else if( strcmp( pModel, "2024" ) == 0 )
              pM->instrument = mi2024;
            else
              pM->instrument = MINONE; 
          }
               /* 3.11 Check for possible Racal naming variants */
               
          else if( ( strcmp( pManuf, "RACAL" ) == 0 )
                || ( strcmp( pManuf, "RACAL INSTRUMENTS" ) == 0 )
                || ( strcmp( pManuf, "RI" ) == 0 ) )
          {
            pModel = strtok( NULL, "," );
            if( strcmp( pModel, "3271" ) == 0 )
              pM->instrument = ri3271;
            else
              pM->instrument = MINONE; 
          }
          else if( strcmp( pManuf, "IFR" ) == 0 ) /* 2.03 */
          {
            pModel = strtok( NULL, "," );
            if( strcmp( pModel, "2023A" ) == 0 )
              pM->instrument = ifr2023A;
            else if( strcmp( pModel, "2023B" ) == 0 )
              pM->instrument = ifr2023B;
            else if( strcmp( pModel, "2025" ) == 0 )
              pM->instrument = ifr2025;
            else
              pM->instrument = MINONE; 
          }
          else
            pM->instrument = MINONE;
        }
        if( pM->instrument == MINONE )
          status = ifr2025_ERROR_INST_NOT_SUPP;
      }
    }
  }
  
  if( status >= VI_SUCCESS )
  {
    if( pM->instrument != MINONE )
    {
      status = viPrintf( *vi, "*OPT?\n" );
      if( status >= VI_SUCCESS )
      {
        ViChar  buffer[256];
      
        status = viScanf( *vi, "%t", buffer );
        if( status >= VI_SUCCESS )
        {
          if( strstr( buffer, "HIGH POWER" ) != NULL )
            pM->highPower = VI_TRUE;
          else
            pM->highPower = VI_FALSE;
          if( strstr( buffer, "SINAD" ) != NULL )
            pM->sinad = VI_TRUE;
          else
            pM->sinad = VI_FALSE;
        }
      }
    }
  }
  
    /* call the revision query routine to update the version data */
  if( status >= VI_SUCCESS )
  {
    ViChar  s1[256], s2[256];
    
	DelayWithEventProcessing(0.3);  
	
    status = ifr2025_revision_query (*vi, s1, s2);
  }
  
  if( status >= VI_SUCCESS )
  {
    ViInt16 u;
    ViInt16 t;
    ViInt16 s;
    ViInt16 m;
    ViReal64 l;
    ViReal64 f;
    
	DelayWithEventProcessing(0.3);
	
    status = ifr2025_carrierQuery (*vi, &f, &l, &u, &s, &t, &m);
    if( status >= VI_SUCCESS )
      pM->rfType = t;
  }
  
  
  /* End of function status check. */
  if( status < VI_SUCCESS )
  {
    ifr2025_close( *vi );
    *vi = VI_NULL;
  }

  return status;
}

/*  =======================================================================*/
/*  Function : ifr2025_close()
    Purpose  : closes the instrument session and associated RM session */
/*  =======================================================================*/
ViStatus _VI_FUNC ifr2025_close( ViSession vi )
{
  ViStatus  status;
  ViSession defRM;
  sgData    *pM;

    /* get the handle for the RM session that opened this device */
    /* this should only fail if the vi is invalid, which if it is means */
    /* we can't go further */
  status = viGetAttribute( vi, VI_ATTR_USER_DATA, (sgData *) &pM );
  if( status >= VI_SUCCESS )
  {
    if( pM != NULL )
      free( pM );
  }
  
  status = viGetAttribute( vi, VI_ATTR_RM_SESSION, (ViAddr) &defRM );
  if( status < VI_SUCCESS )
    return status;

  status = viClose( vi );
  viClose( defRM );
  return status;
}

/*  ======================================================================= */
/*  Function : ifr2025_reset()
    Purpose  : issues a *RST command to the instrument                      */
/*  ======================================================================= */
ViStatus _VI_FUNC ifr2025_reset( ViSession vi )
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf( vi, "*RST\n" );

  RETURN( vi, status );
}

/*  ======================================================================= */
/*  Function : ifr2025_self_test()
    Purpose  : issues a *TST command and returns the result as code/string  */
/*  ======================================================================= */
ViStatus _VI_FUNC ifr2025_self_test( ViSession vi,
                                    ViInt16  *result,
                                    ViString message )
{
  ViStatus status = VI_SUCCESS;
  ViUInt32 response;

  status = viPrintf( vi, "*TST?\n" );
  if( status < VI_SUCCESS )
    RETURN( vi, status );

  status = viScanf( vi, "%ld%*t", &response );
  if( status < VI_SUCCESS )
    RETURN( vi, status );

  if( response == 0 )
  {
    *result = 0;
    strcpy( message , "Self Test OK" );
  }
  else
  {
    *result = -1;
    strcpy( message , "Self Test Failure" );
  }
  RETURN( vi, status );
}

/*  ======================================================================= */
/*  Function : ifr2025_revision_query()
    Purpose  : returns both the driver and instrument versions as strings   */
/*  ======================================================================= */
ViStatus _VI_FUNC ifr2025_revision_query( ViSession vi,
                                         ViString  driver,
                                         ViString  instrument )
{
  ViChar  idString[256];
  ViPChar p, q;
  ViStatus status = VI_SUCCESS;
  sgData    *pM; /* 2.04 */

    /* get the handle for the RM session that opened this device */
  status = viGetAttribute( vi, VI_ATTR_USER_DATA, (sgData *) &pM );

  *driver = '\0';
  *instrument = '\0';

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "*IDN?\n" );
  
  if( status >= VI_SUCCESS )
  {
    status = viScanf( vi, "%t", idString );
    if( status >= VI_SUCCESS )
    {
      p = strtok( idString, "," );  /* MARCONI INSTRUMENTS */
      p = strtok( NULL, "," );      /* 3002 */
      p = strtok( NULL, "," );      /* 811152/011 */
      p = strtok( NULL, ",\n" );      /* 44533/445/01.00 */
      if( p != NULL )
      {
        strcpy( instrument, p  );
        strcpy( driver, ifr2025_DRIVER_REVISION );
        q = strrchr( p, '/' );
        if( q != NULL )
        {
          q++;
          pM->version = atof( q );
        }
        else
          pM->version = 0.0;
      }
    }
  }
  RETURN( vi, status );
}

/*  ======================================================================= */
/*  Function : ifr2025_error_query()
    Purpose  : extracts an error message from the instrument                */
/*  ======================================================================= */
ViStatus _VI_FUNC ifr2025_error_query( ViSession vi,
                                      ViPInt32  error,
                                      ViString  message )
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf( vi, "ERROR?\n" );
  if( status < VI_SUCCESS )
    return status ;

  status = viScanf( vi, "%ld,%t", error, message);

  return status;
}

/*  ======================================================================= */
/*  Function : ifr2025_error_message()
    Purpose  : converts an ifr2025_xxx error code to a string                */
/*  ======================================================================= */
ViStatus _VI_FUNC ifr2025_error_message( ViSession vi,
                                        ViInt32   error,
                                        ViString  message )
{
  ViStatus status = VI_SUCCESS;
  int       i;
  
  i = 0;
  while( miErrors[i].error != VI_SUCCESS )
  {
    if( miErrors[i].error == error )
    {
      strcpy( message, miErrors[i].msg );
      return VI_SUCCESS;
    }
    i++;
  }
  
  if( miErrors[i].error == VI_SUCCESS )
      status = viStatusDesc( vi, error, message );

  return status;
}

/*=========================================================================*/

/*----------- INSERT INSTRUMENT-DEPENDENT ROUTINES HERE -------------------*/

/*=========================================================================*/
/* Set RF outout level State (ie, On or Off)                                */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfLevel(ViSession vi, ViReal64 level,ViInt16 u )
{
  ViStatus status = VI_SUCCESS;
  ViReal64  *min,*max;
  sgData    *pM;

  switch( u )
  {
    case ifr2025_RFUNIT_DBM:
    case ifr2025_RFUNIT_DBV:
    case ifr2025_RFUNIT_DBMV:
    case ifr2025_RFUNIT_DBUV:
    case ifr2025_RFUNIT_V:
    case ifr2025_RFUNIT_MV:
    case ifr2025_RFUNIT_UV:
      break;

    default:
      status = ifr2025_ERROR_RFUNITS;
  }

  if( status >= VI_SUCCESS )
  {
    status = viGetAttribute( vi, VI_ATTR_USER_DATA, &pM );
  }
  
  if( status >= VI_SUCCESS )
  {
    
    if( pM->rfType == ifr2025_RFTYPE_EMF )
    {
      min = emfMin;
      max = emfMax;
    }
    else
    {
      min = pdMin;
      max = pdMax;
    }
    
    status = CHECKVAL(level, min[u], max[u], ifr2025_ERROR_RFLEVEL );
    
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "RFLV:VALUE %f%s\n", level, units[u] );
    
  RETURN( vi, status);
}

/*=========================================================================*/
/* Set RF output level State (ie, On or Off)                               */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfState(ViSession vi, ViInt16 rfState)
{
  ViStatus status = VI_SUCCESS;

  switch( rfState)
  {
    case ifr2025_ON:
    case ifr2025_OFF:
      status = viPrintf( vi, "RFLV:%s\n", state[rfState] );
      break;

    default:
      status = ifr2025_ERROR_ONOFF;
  }

  RETURN( vi, status);
}

/*=========================================================================*/
/* Set RF output level type (ie, emf or pd)                                */
/* type  =  ifr2025_RFTYPE_EMF                                              */
/*       or ifr2025_RFTYPE_PD                                               */
/* units =  ifr2025_RFUNIT_DBM
            ifr2025_RFUNIT_DBV
            ..
            ifr2025_RFUNIT_UV                                               */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfType (ViSession vi, ViInt16 type, ViInt16 rfUnits)
{
  ViStatus  status = VI_SUCCESS;
  sgData    *pM;

  switch( type )
  {
    case ifr2025_RFTYPE_EMF:
    case ifr2025_RFTYPE_PD:
      status = viPrintf( vi, "RFLV:TYPE %s\n", rfType[type] );
      break;

    default:
      status = ifr2025_ERROR_RFTYPE;
  }

  switch( rfUnits )
  {
    case ifr2025_RFUNIT_DBM:
    case ifr2025_RFUNIT_DBV:
    case ifr2025_RFUNIT_DBMV:
    case ifr2025_RFUNIT_DBUV:
    case ifr2025_RFUNIT_V:
    case ifr2025_RFUNIT_MV:
    case ifr2025_RFUNIT_UV:
      status = viPrintf( vi, "RFLV:UNITS %s\n", units[rfUnits] );
      break;
    
    default:
      status = ifr2025_ERROR_RFUNITS;
  }
  
  if( status >= VI_SUCCESS )
  {
    status = viGetAttribute( vi, VI_ATTR_USER_DATA, &pM );
    pM->rfType = type;
  }
  
  RETURN( vi, status );
}


/*=========================================================================*/
/* Set the RF frequency ( value is in Hz )                                 */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfFreq (ViSession vi, ViReal64 freq )
{
  ViStatus status = VI_SUCCESS;
  ViReal64 freqMax;
  sgData    *pM;

  status = viGetAttribute( vi, VI_ATTR_USER_DATA, &pM );
  if( status >= VI_SUCCESS )
  {
    switch( pM->instrument )
    {
      case mi3002:
      case mi2024:
      case ri3271:
        freqMax = mi3002_FREQ_MAX;
        break;
      
      case mi2023:
      case ifr2023A:
        freqMax = mi2023_FREQ_MAX;
        break;
      
      case ifr2023B:
        freqMax = ifr2023B_FREQ_MAX;
        break;
      
      case ifr2025:
        freqMax = ifr2025_FREQ_MAX;
        break;
      
      default:
        freqMax = mi3002_FREQ_MAX;
        break;
    }
    status = CHECKVAL(freq, ifr2025_FREQ_MIN, freqMax, ifr2025_ERROR_RFFREQ );
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "CFRQ %fHz\n", freq );

  RETURN(vi, status );
}


/*=========================================================================*/
/* Atten lock/unlock                                                       */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_attenLockUnlock (ViSession vi, ViInt16 attenLock)
{
  ViStatus status = VI_SUCCESS;

  switch( attenLock )
  {
    case ifr2025_ATTEN_UNLOCK:
    case ifr2025_ATTEN_LOCK:
      status = viPrintf( vi, "ATTEN:%s\n", atten_tab[attenLock] );
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }
  RETURN( vi, status );
}



/*=========================================================================*/
/* Carrier Phase                                                       */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfPhase (ViSession vi, ViReal64 phase)
{
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(phase, ifr2025_PHASE_MIN, ifr2025_PHASE_MAX, ifr2025_ERROR_PHASE );

  /* send phase value */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "CFRQ:PHASE %fDEG\n", phase );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Set the RF limit                                                    */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfLimit (ViSession vi, ViReal64 level, ViInt16 u, ViInt16 state)
{
  ViStatus status = VI_SUCCESS;
  ViReal64  *min,*max;
  sgData    *pM;

  switch( u )
  {
    case ifr2025_RFUNIT_DBM:
    case ifr2025_RFUNIT_DBV:
    case ifr2025_RFUNIT_DBMV:
    case ifr2025_RFUNIT_DBUV:
    case ifr2025_RFUNIT_V:
    case ifr2025_RFUNIT_MV:
    case ifr2025_RFUNIT_UV:
      break;

    default:
      status = ifr2025_ERROR_RFUNITS;
  }

  if( status >= VI_SUCCESS )
  {
    status = viGetAttribute( vi, VI_ATTR_USER_DATA, &pM );
  }
  
  if( status >= VI_SUCCESS )
  {
    
    if( pM->rfType == ifr2025_RFTYPE_EMF )
    {
      min = emfMin;
      max = emfMax;
    }
    else
    {
      min = pdMin;
      max = pdMax;
    }
    
    status = CHECKVAL(level, min[u], max[u], ifr2025_ERROR_RFLEVEL );
    
  }

  if( status >= VI_SUCCESS )
  {
    if( state == ifr2025_ENABLE )
      status = viPrintf( vi, "RFLV:LIMIT:VALUE %f%s;ENABLE\n", level, units[u] );
    else
      status = viPrintf( vi, "RFLV:LIMIT:DISABLE\n" );
  }
  
  RETURN( vi, status);
}

/*=========================================================================*/
/* Set the RF level offset                                                 */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rfOffset (ViSession vi, ViReal64 level, ViInt16 state)
{
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(level, ifr2025_RFOFFSET_MIN, ifr2025_RFOFFSET_MAX, ifr2025_ERROR_RFOFFSET );

  /* send phase value */
  if( status >= VI_SUCCESS )
  {
    if( state == ifr2025_ENABLE )
      status = viPrintf( vi, "RFLV:OFFS %f;OFFS:ENABLE\n", level );
    else
      status = viPrintf( vi, "RFLV:OFFS:DISABLE\n" );
  }
  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set Mod Phase                                                           */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modPhase (ViSession vi, ViReal64 phase)
{
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(phase, ifr2025_PHASE_MIN, ifr2025_PHASE_MAX, ifr2025_ERROR_PHASE );

  /* send phase value */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FM:MODF:PHASE %fDEG\n", phase );

  RETURN( vi, status );
}


/*=========================================================================*/
/* Select modulation mode                                                  */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modMode(ViSession vi, ViInt16 modMode, ViInt16 pulseEnable)
{
  ViStatus status = VI_SUCCESS;

  switch( modMode )
  {
    case ifr2025_MODE_AM:
    case ifr2025_MODE_FM:
    case ifr2025_MODE_PM:
    case ifr2025_MODE_AMFM:
    case ifr2025_MODE_AMPM:
    case ifr2025_MODE_2FSK:
    case ifr2025_MODE_4FSK:
      break;

    default:
      status = ifr2025_ERROR_MODMODE;
  }

  switch( pulseEnable )
  {
    case ifr2025_PULSE_OFF:
    case ifr2025_PULSE_ON:
      break;

    default:
      status = ifr2025_ERROR_PULSE;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "MODE %s%s\n", mod_mode_tab[modMode], pulse[pulseEnable] );

  RETURN( vi, status );
}


/*=========================================================================*/
/* Set AM parameters                                                       */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modAM (ViSession vi,
                                 ViInt16 channel,
                                 ViReal64 depth,
                                 ViReal64 modRate,
                                 ViInt16 waveform,
                                 ViInt16 source,
                                 ViBoolean amState)
{
  ViStatus status = VI_SUCCESS;

  if( (channel != 1) && ( channel != 2 ) )
    status = ifr2025_ERROR_MODCHAN;

  status = CHECKVAL(depth, ifr2025_AMDEPTH_MIN, ifr2025_AMDEPTH_MAX, ifr2025_ERROR_DEPTH );

  switch( source )
  {
    case ifr2025_SRC_INT:
    case ifr2025_SRC_EXTAC:
    case ifr2025_SRC_EXTALC:
    case ifr2025_SRC_EXTDC:
      break;

    default:
      status = ifr2025_ERROR_SOURCE;
  }

  if( (amState != ifr2025_ON) && ( amState != ifr2025_OFF ) )
    status = ifr2025_ERROR_STATE;

  if (source == ifr2025_SRC_INT )
  {
    status = CHECKVAL(modRate, ifr2025_AMRATE_MIN, ifr2025_AMRATE_MAX, ifr2025_ERROR_RATE );

    switch( waveform )
    {
      case ifr2025_MODF_SIN:
      case ifr2025_MODF_TRI:
      case ifr2025_MODF_SQR:
        break;

      default:
        status = ifr2025_ERROR_WAVEFORM;
    }

    if( status >= VI_SUCCESS )
      status = viPrintf( vi, "AM%d:MODF %fHZ\n", channel, modRate);
    
    if( status >= VI_SUCCESS )
      status = viPrintf( vi, "AM%d:MODF:%s\n", channel, mod_waveform[waveform]);
  }

  /* send AM depth */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "AM%d %fPCT\n", channel, depth );

  /* send AM state */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "AM%d:%s\n", channel, state[amState]);
  
  /* send AM source*/
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "AM%d:%s\n", channel, mod_source[source]);

  RETURN( vi, status );
}


/*=========================================================================*/
/* Set Phase Modulation Parameters                                         */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modPM ( ViSession vi,
                                 ViInt16 channel,
                                 ViReal64 deviation,
                                 ViReal64 modRate,
                                 ViInt16 waveform,
                                 ViInt16 source,
                                 ViBoolean pmState )
{
  ViStatus status = VI_SUCCESS;

  if( (channel != 1) && ( channel != 2 ) )
    status = ifr2025_ERROR_MODCHAN;

  status = CHECKVAL(deviation, ifr2025_PMDEVIATION_MIN, ifr2025_PMDEVIATION_MAX, ifr2025_ERROR_DEPTH );

  switch( source )
  {
    case ifr2025_SRC_INT:
    case ifr2025_SRC_EXTAC:
    case ifr2025_SRC_EXTALC:
    case ifr2025_SRC_EXTDC:
      break;

    default:
      status = ifr2025_ERROR_SOURCE;
  }

  if( (pmState != ifr2025_ON) && ( pmState != ifr2025_OFF ) )
    status = ifr2025_ERROR_ONOFF;

  if (source == ifr2025_SRC_INT)
  {          /* INT source */

    status = CHECKVAL(modRate, ifr2025_PMRATE_MIN, ifr2025_PMRATE_MAX, ifr2025_ERROR_RATE );

    switch( waveform )
    {
      case ifr2025_MODF_SIN:
      case ifr2025_MODF_TRI:
      case ifr2025_MODF_SQR:
        break;

      default:
        status = ifr2025_ERROR_WAVEFORM;
    }

    if( status >= VI_SUCCESS )
      status = viPrintf( vi, "PM%d:MODF %fHZ\n", channel, modRate);

    if( status >= VI_SUCCESS )
      status = viPrintf( vi, "PM%d:MODF:%s\n", channel, mod_waveform[waveform]);
  }

  /* send PM deviation */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "PM%d %f\n", channel, deviation );

  /* send PM state */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "PM%d:%s\n", channel, state[pmState]);

  /* send PM source*/
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "PM%d:%s\n", channel, mod_source[source]);

  RETURN( vi, status );
}


/*=========================================================================*/
/* Set Frequency Modulation Parameters                                     */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modFM ( ViSession vi,
                                 ViInt16 channel,
                                 ViReal64 deviation,
                                 ViReal64 modRate,
                                 ViInt16 waveform,
                                 ViInt16 source,
                                 ViBoolean fmState )
{
  ViStatus status = VI_SUCCESS;

  if( (channel != 1) && ( channel != 2 ) )
    status = ifr2025_ERROR_MODCHAN;

  status = CHECKVAL(deviation, ifr2025_FMDEVIATION_MIN, ifr2025_FMDEVIATION_MAX, ifr2025_ERROR_DEPTH );

  switch( source )
  {
    case ifr2025_SRC_INT:
    case ifr2025_SRC_EXTAC:
    case ifr2025_SRC_EXTALC:
    case ifr2025_SRC_EXTDC:
      break;

    default:
      status = ifr2025_ERROR_WAVEFORM;
  }

  if( (fmState != ifr2025_ON) && ( fmState != ifr2025_OFF ) )
    status = ifr2025_ERROR_ONOFF;

  if (source == ifr2025_SRC_INT)
  {          /* INT source */

    status = CHECKVAL(modRate, ifr2025_FMRATE_MIN, ifr2025_FMRATE_MAX, VI_ERROR_PARAMETER4 );

    switch( waveform )
    {
      case ifr2025_MODF_SIN:
      case ifr2025_MODF_TRI:
      case ifr2025_MODF_SQR:
        break;

      default:
        status = ifr2025_ERROR_WAVEFORM;
    }

    if( status >= VI_SUCCESS )
      status = viPrintf( vi, "FM%d:MODF %fHZ\n", channel, modRate);
  
    if( status >= VI_SUCCESS )
      status = viPrintf( vi, "FM%d:MODF:%s\n", channel, mod_waveform[waveform]);
  }

  /* send FM deviation */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FM%d %f\n", channel, deviation );

  /* send FM state */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FM%d:%s\n", channel, state[fmState]);

  /* send PM source*/
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FM%d:%s\n", channel, mod_source[source]);

  RETURN( vi, status );
}



/*=========================================================================*/
/* Set FSK Ext                                                                 */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_setFSK (ViSession vi,
                                 ViReal64 deviation,
                                 ViBoolean fmState)
{
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(deviation, ifr2025_FMDEVIATION_MIN, ifr2025_FMDEVIATION_MAX, VI_ERROR_PARAMETER2 );

  if( (fmState != ifr2025_ON) && ( fmState != ifr2025_OFF ) )
    status = ifr2025_ERROR_ONOFF;


  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FM:DEVN %f\n", deviation );

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FM1:%s\n", state[fmState] );

  RETURN( vi, status );
}


/*=========================================================================*/
/* Global Mod and Pulse On Off States                                      */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modControl(ViSession  vi, ViBoolean globalMod, ViBoolean pulseState)
{
  ViStatus status = VI_SUCCESS;

  switch( globalMod )
  {
    case ifr2025_ON:
    case ifr2025_OFF:
      break;

    default:
      status = ifr2025_ERROR_ONOFF;
  }

  switch( pulseState )
  {
    case ifr2025_ON:
    case ifr2025_OFF:
      break;

    default:
      status = ifr2025_ERROR_ONOFF;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "MOD:%s\n", state[globalMod] );

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "PULSE:%s\n", state[pulseState] );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Set Sweep                                                               */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_sweep (ViSession vi,
                                ViReal64 startFreq,
                                ViReal64 stopFreq,
                                ViReal64 stepFreq,
                                ViReal64 stepTime,
                                ViReal64 logInc )
{
  sgData    *pM;
  ViReal64 freqMax;
  ViStatus  status = VI_SUCCESS;

  
  status = viGetAttribute( vi, VI_ATTR_USER_DATA, (sgData *) &pM );

  if( status >= VI_SUCCESS )
  {
    switch( pM->instrument )
    {
      case mi3002:
      case mi2024:
      case ri3271:
        freqMax = mi3002_FREQ_MAX;
        break;
      
      case mi2023:
      case ifr2023A:
        freqMax = mi2023_FREQ_MAX;
        break;
      
      case ifr2023B:
        freqMax = ifr2023B_FREQ_MAX;
        break;
      
      case ifr2025:
        freqMax = ifr2025_FREQ_MAX;
        break;
      
      default:
        freqMax = mi3002_FREQ_MAX;
        break;
    }
  }
  if( status >= VI_SUCCESS )
    status = CHECKVAL(startFreq, ifr2025_FREQ_MIN, freqMax, ifr2025_ERROR_STARTFREQ );
  
  if( status >= VI_SUCCESS )
    status = CHECKVAL(stopFreq, ifr2025_FREQ_MIN, freqMax, ifr2025_ERROR_STOPFREQ );

  if( status >= VI_SUCCESS )
    status = CHECKVAL(stepFreq, ifr2025_STEP_FREQ_MIN, freqMax, ifr2025_ERROR_STEPFREQ );

  if( status >= VI_SUCCESS )
    status = CHECKVAL(stepTime, ifr2025_STEP_TIME_MIN, ifr2025_STEP_TIME_MAX, ifr2025_ERROR_STEPTIME );
  
  if( status >= VI_SUCCESS )
    status = CHECKVAL(logInc, 0.01, 50.00, ifr2025_ERROR_LOGINC );


  if( status >= VI_SUCCESS )
  {
    switch( pM->instrument )
    {
      case mi3002:
      case ri3271:
        status = viPrintf( vi,
                           "SWEEP:CFRQ:START %f;STOP %f;INC %f;TIME %f;LOGINC %f\n",
                           startFreq,
                           stopFreq,
                           stepFreq,
                            stepTime,
                            logInc );
        break;
      
      default:
        status = viPrintf( vi,
                           "SWEEP:CFRQ:START %f;STOP %f;INC %f;TIME %f\n",
                           startFreq,
                           stopFreq,
                           stepFreq,
                           stepTime );
        break;
    }
  }
  
  RETURN( vi, status );
}


/*=========================================================================*/
/* Set Sweep Mode                                                          */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_sweepMode (ViSession vi,
                                         ViInt16 carrierMode,
                                         ViInt16 sweepMode,
                                         ViInt16 triggerMode,
                                         ViInt16 triggerSource,
                                         ViInt16 type )
{
  ViStatus status = VI_SUCCESS;
  sgData  *pM;

  status = viGetAttribute( vi, VI_ATTR_USER_DATA, &pM );
  
  switch( carrierMode )
  {
    case ifr2025_SWEPT:
    case ifr2025_FIXED:
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  switch( sweepMode )
  {
    case ifr2025_SINGLE:
    case ifr2025_CONTINUOUS:
      break;

    default:
      status = VI_ERROR_PARAMETER3;
  }

  switch( triggerMode )
  {
    case ifr2025_TRIG_OFF:
    case ifr2025_TRIG_START:
    case ifr2025_TRIG_STARTSTOP:
    case ifr2025_TRIG_STEP:
      break;

    default:
      status = VI_ERROR_PARAMETER4;
  }

  switch( triggerSource )
  {
    case ifr2025_TRIG_HOLD:
    case ifr2025_TRIG_TTL0:
    case ifr2025_TRIG_TTL1:
    case ifr2025_TRIG_TTL2:
    case ifr2025_TRIG_TTL3:
    case ifr2025_TRIG_TTL4:
    case ifr2025_TRIG_TTL5:
    case ifr2025_TRIG_TTL6:
    case ifr2025_TRIG_TTL7:
    case ifr2025_TRIG_BUS:
    case ifr2025_TRIG_FRONT:
      break;

    default:
      status = VI_ERROR_PARAMETER5;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "CFRQ:MODE %s\n", cfrq_mode_tab[carrierMode]);

  if( status >= VI_SUCCESS )
    /* send sweep mode  */
    status = viPrintf( vi, "SWEEP:MODE %s;TRIG %s\n",
                            sweep_mode_tab[sweepMode],
                            sweep_trig_tab[triggerMode] );
  
  if( status >= VI_SUCCESS )
  {
    switch( pM->instrument )
    {
      case mi3002:
      case ri3271:
        status = viPrintf( vi, "TRIGGER:SOURCE %s\n", trig_src_tab[triggerSource]);
        if( status >= VI_SUCCESS )
          status = viPrintf( vi, "SWEEP:TYPE %s\n", sweepType[type] );
        break;
      
      default:
        break;
    }
  }
  
  RETURN( vi, status );
}



/*=========================================================================*/
/* Set Sweep Mode and Control                                              */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_sweepRun (ViSession vi, ViInt16 sweepControl)
{
  ViStatus status = VI_SUCCESS;

  switch( sweepControl )
  {
    case ifr2025_SWEEP_GO:
    case ifr2025_SWEEP_HALT:
    case ifr2025_SWEEP_CONT:
    case ifr2025_SWEEP_RESET:
    case ifr2025_SWEEP_RETN:
    case ifr2025_SWEEP_XFER:
    case ifr2025_SWEEP_UP:
    case ifr2025_SWEEP_DOWN:
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  /* send sweep control */
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "SWEEP:%s\n", sweep_control_tab[sweepControl]);

  RETURN( vi, status );
}

/*=========================================================================*/
/* Memory Store                                                            */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_memStore (ViSession vi, ViInt16 type, ViInt16 num)
{
  ViStatus status = VI_SUCCESS;

  switch( type )
  {
    case ifr2025_MEM_CFRQ:
      status = CHECKVAL(num, 0, 99, VI_ERROR_PARAMETER3 );
      break;

    case ifr2025_MEM_FULL:
      status = CHECKVAL(num, 100, 199, VI_ERROR_PARAMETER3 );
      break;

    case ifr2025_MEM_RAM:
      status = CHECKVAL(num, 200, 299, VI_ERROR_PARAMETER3 );
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "STO:MEM %d\n", num );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Memory Recall                                                           */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_memRecall (ViSession vi, ViInt16 type, ViInt16 num)
{
  ViStatus status = VI_SUCCESS;

  switch( type )
  {
    case ifr2025_MEM_CFRQ:
      status = CHECKVAL(num, 0, 99, VI_ERROR_PARAMETER3 );
      break;

    case ifr2025_MEM_FULL:
      status = CHECKVAL(num, 100, 199, VI_ERROR_PARAMETER3 );
      break;

    case ifr2025_MEM_RAM:
      status = CHECKVAL(num, 200, 299, VI_ERROR_PARAMETER3 );
      break;

    case ifr2025_MEM_ALL:
      if( ( num < 0 ) || (( num > 299 ) && ( num != 999)) )
        status = VI_ERROR_PARAMETER3;
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "RCL:MEM %d\n", num );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Memory Erase Stores                                                     */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_memErase (ViSession vi, ViInt16 type)
{
  ViStatus status = VI_SUCCESS;

  switch( type )
  {
    case ifr2025_MEM_CFRQ:
    case ifr2025_MEM_FULL:
    case ifr2025_MEM_RAM:
    case ifr2025_MEM_ALL:
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "ERASE:%s\n", mem[type] );

  RETURN( vi, status );
}

/*========================================================================*/
/* Set memory protection                                                  */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_memProtection (ViSession vi,
                                       ViBoolean mpState,
                                       ViInt16 start,
                                       ViInt16 stop)
{
  ViStatus status = VI_SUCCESS;

  switch( mpState )
  {
    case ifr2025_OFF:
    case ifr2025_ON:
      break;

    default:
      return VI_ERROR_PARAMETER2;
  }

  status = CHECKVAL(start, 0, 299, VI_ERROR_PARAMETER3 );

  status = CHECKVAL(stop, start, 299, VI_ERROR_PARAMETER4 );


  status = viPrintf( vi, "MPROT:START %d;STOP %d;%s\n", start, stop, state[mpState]);

  RETURN( vi, status );
}


/*========================================================================*/
/* Set memory Download                                                    */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_memDownload(ViSession vi, ViBoolean mdState)
{
  ViStatus status = VI_SUCCESS;

  switch( mdState )
  {
    case ifr2025_ENABLE:
    case ifr2025_DISABLE:
      break;

    default:
      return VI_ERROR_PARAMETER2;
  }


  status = viPrintf( vi, "OUTPUT:%s\n", rf_out[mdState]);

  RETURN( vi, status );
}


/*========================================================================*/
/* Set memory trigger                                                     */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_memTrigger (ViSession vi, ViBoolean mtState)
{
  ViStatus status = VI_SUCCESS;

  switch( mtState )
  {
    case ifr2025_ON:
    case ifr2025_OFF:
      break;

    default:
      return VI_ERROR_PARAMETER2;
  }


  status = viPrintf( vi, "MTRIG:%s\n", state[mtState]);

  RETURN( vi, status );
}


/*========================================================================*/
/* Select memory sequence                                                 */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_memSelectSeq (ViSession vi,
                                         ViInt16  sequence,
                                         ViInt16  *start,
                                         ViInt16  *stop)
{
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(sequence, 0, 9, VI_ERROR_PARAMETER2 );

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "MSEQ:MODE %s\n", seq_mode[sequence]);

  if (sequence == 0)
  {
    *start = 0;
    *stop = 299;
    return VI_SUCCESS;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "MSEQ:%s?\n", seq_mode[sequence]);

  if( status >= VI_SUCCESS )
    status = viScanf(vi, "%*s %d %*s %d %*t", start, stop );

  RETURN( vi, status );
}



/*========================================================================*/
/* Set memory sequence                                                    */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_memSequence (ViSession vi, ViInt16 seq, ViInt16 start, ViInt16 stop)
{
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(seq, 1, 9, VI_ERROR_PARAMETER2 );

  status = CHECKVAL(start, 0, 299, VI_ERROR_PARAMETER3 );

  status = CHECKVAL(stop, start, 299, VI_ERROR_PARAMETER4 );

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "MSEQ:%s:START %d;STOP %d\n", seq_mode[seq], start, stop);

  RETURN( vi, status );
}


/*========================================================================*/
/* Step through sequence                                                  */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_memStepSeq (ViSession vi, ViInt16 direction, ViInt16 *storeNumber)
{
  ViStatus status = VI_SUCCESS;

  char buff[20];

  switch( direction )
  {
    case ifr2025_MEM_UP:
    case ifr2025_MEM_DOWN:
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "RCL:%s\n", mem_dir[direction]);

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "RCL?\n" );

  if( status >= VI_SUCCESS )
    status = viScanf(vi, "%s% d% %*t", buff, storeNumber );

  RETURN( vi, status );
}

/*========================================================================*/
/* DCFM Null                                                              */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_dcfmNull (ViSession vi)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf (vi, "DCFMNL\n" );

  RETURN( vi, status );
}


/*========================================================================*/
/* Reset reverse power protection trip                                    */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_rppReset (ViSession vi)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf (vi, "RPP:RESET\n" );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Select internal or external frequency standard type.                    */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_setFSTD (ViSession vi, ViInt16 freqStandard)
{
  ViStatus status = VI_SUCCESS;

  switch( freqStandard )
  {
    case ifr2025_FSTD_INT:
    case ifr2025_FSTD_EXT10DIR:
    case ifr2025_FSTD_EXT1IND:
    case ifr2025_FSTD_INT10OUT:
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "FSTD %s\n", fstd_type_tab[freqStandard]);

  RETURN( vi, status );
}

/*=========================================================================*/
/* Counts the number of times that the Reset Reverse Protection has        */
/* tripped.                                                                 */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_rppQuery( ViSession vi, ViInt16 *rppTripped, ViInt16 *rppCount)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf(vi, "RPP:TRIPPED?\n" );

  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%d %*t", rppTripped );

  if( status >= VI_SUCCESS )
    status = viPrintf(vi, "RPP:COUNT?\n" );

  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%d %*t", rppCount );

  RETURN( vi, status );
}


/*========================================================================*/
/* Reset elapsed operating hours to zero.                                 */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_resetElapsedTime (ViSession vi)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf (vi, "ELAPSED:RESET\n" );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Counts the number of elapsed operating hours since the last reset.      */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_elapsedTimeQuery (ViSession vi, ViReal64 *elapsedHours)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf (vi, "ELAPSED?\n" );

  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%lf %*t", elapsedHours );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Counts the total number of operating hours.                             */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_operatingTimeQuery (ViSession vi, ViReal64 *totalHours)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf (vi, "OPER?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%lf %*t", totalHours );

  RETURN( vi, status );
}

/*=========================================================================*/
/* Power on from memory                                                    */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_memPowerUp (ViSession vi, ViInt16 mode, ViInt16 memoryNumber)
{
  ViStatus status = VI_SUCCESS;

  switch( mode )
  {
    case ifr2025_MEMORY:
    case ifr2025_FACTORY:
      break;

    default:
      status = VI_ERROR_PARAMETER2;
  }

  if( mode==ifr2025_MEMORY )
  	status = CHECKVAL(memoryNumber, 0, 199, VI_ERROR_PARAMETER3 );

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "POWUP:MODE %s\n", powup[mode]);

  if( status >= VI_SUCCESS && mode==ifr2025_MEMORY )
    status = viPrintf( vi, "POWUP:MEM %d\n", memoryNumber);

  RETURN( vi, status );
}


/*=========================================================================*/
/*  QUERY:Get the carrier settings                                         */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_carrierQuery (ViSession  vi,
                                       ViReal64   *freq,
                                       ViReal64   *l,      /* level */
                                       ViInt16    *rfUnit,
                                       ViInt16    *rfState,
                                       ViInt16    *rfType,
                                       ViInt16    *mode)
{
  ViStatus  status = VI_SUCCESS;
  char      buffer[128];
  ViReal64  inc;

  
  status = viPrintf( vi, "CFRQ?\n" );

  /*  :CFRQ:VALUE 10000.0;INC 2500.0;MODE FIXED */
  /* Extract FREQ, INC and MODE */
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %lf %*s %lf %*s %t\n", freq, &inc, buffer );
    if ( status >= VI_SUCCESS )
    {
      if( strncmp(buffer, "FIXED", 5) == 0 )
        *mode = ifr2025_FIXED;
      else
        *mode = ifr2025_SWEPT;

      status = viPrintf( vi, "RFLV?\n" );
      if ( status >= VI_SUCCESS )
      {
        ViChar    u[128];
        ViChar    t[128];
        ViChar    s[128];
        ViReal64  i;
        int       j;

        /* :RFLV:UNITS DBM;TYPE PD;VALUE 101.1;INC 2.0;ON */
        /* Extract      ^        ^         ^        ^   ^ */
        status = viScanf( vi,
                          "%*s %s %s %lf %*s %lf;%t",
                                u, t,  l,     &i, s );
        if ( status >= VI_SUCCESS )
        {
          /* process units - default to DBM */
          *rfUnit = 0;

          for( j=0; j<7; j++ )
            if( strncmp( u, units[j], strcspn(u, ";")) == 0 )
            {
              *rfUnit = j;
              break;
            }
          
          /* process type */
          if( strncmp( t, "PD", 2 ) == 0 )
            *rfType = ifr2025_RFTYPE_PD;
          else
            *rfType = ifr2025_RFTYPE_EMF;

          /* process state */
          if( strncmp( s, "ON", 2 ) == 0 )
            *rfState = ifr2025_ON;
          else
            *rfState = ifr2025_OFF;

        }
        if( status >= VI_SUCCESS )
        {
          sgData  *pM;
    
          status = viGetAttribute( vi, VI_ATTR_USER_DATA, (sgData *) &pM );
          if( status >= VI_SUCCESS )
            pM->rfType = *rfType;
        }
      }
    }
    
  RETURN( vi, status );
}

/*=========================================================================*/
/*  QUERY:Get the responses from
    SWEEP:CFRQ?   =  :SWEEP:CFRQ:START x;STOP y;INC z;LOGINC 50.0;TIME 20.0
    and
    SWEEP?        = :SWEEP:MODE CONT;TYPE LOG;TRIG STEP */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_sweepQuery  (ViSession vi,
                                      ViReal64 *startFreq,
                                      ViReal64 *stopFreq,
                                      ViReal64 *stepFreq,
                                      ViReal64 *stepTime,
                                      ViInt16  *mode,
                                      ViInt16  *trigger,
                                      ViReal64 *logInc,
                                      ViInt16  *type )
{
  ViChar    m[32];
  ViChar    y[32];
  ViChar    t[32];
  sgData    *pM;

  ViStatus  status = VI_SUCCESS;


  status = viGetAttribute( vi, VI_ATTR_USER_DATA, (sgData *) &pM );
    
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "SWEEP:CFRQ?\n" );


  if( status >= VI_SUCCESS )
  {
    switch( pM->instrument )
    {
      case mi3002:
      case ri3271:
        status = viScanf( vi,
                          "%*s %lf %*s %lf %*s %lf %*s %lf %*s %lf %*t",
                          startFreq, stopFreq, stepFreq, logInc, stepTime);
        break;
      
      case mi2024:
      case mi2023:
      case ifr2023A:
      case ifr2023B:
      case ifr2025:
      {
        int     count;
        
        if( pM->version < 2.0 )
        {
          status = viScanf( vi,
                            "%*s %lf %*s %lf %*s %lf %*s %lf %*t",
                            startFreq, stopFreq, stepFreq, stepTime);
          *logInc = 0.0;
        }
        else
        {
          status = viScanf( vi,
                            "%*s %lf %*s %lf %*s %lf %*s %lf %*s %lf %*t",
                            startFreq, stopFreq, stepFreq, logInc, stepTime);
        }
        break;
      }
      
      default:
          status = ifr2025_ERROR_INST_NOT_SUPP;
        break;
    }
  }
  
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "SWEEP?\n" );

  if( status >= VI_SUCCESS )
    status = viScanf( vi,
                      "%*s %s %*s %s %t",
                            m,     y, t);

  if( status >= VI_SUCCESS )
  {
    if( strstr(m, "SNGL" ) != NULL )
      *mode = ifr2025_SINGLE;
    else
      *mode = ifr2025_CONTINUOUS;

    if( strstr( y, "LIN") != NULL )
      *type = ifr2025_LIN;
    else
      *type = ifr2025_LOG;
    
    if( strstr( t, "OFF") != NULL )
      *trigger = ifr2025_TRIG_OFF;
    else
    if( strstr( t, "START" ) != NULL )
      *trigger = ifr2025_TRIG_START;
    else
    if( strstr( t, "STARTSTOP" ) != NULL )
      *trigger = ifr2025_TRIG_STARTSTOP;
    else
      *trigger = ifr2025_TRIG_STEP;
  }
  
  RETURN( vi, status );
}

/*=========================================================================*/
/*  Get the responses from
    xM? =  :FM:DEVN 2500.0;INT ;ON ;INC 1000.0
           :AM:DEPTH 91.0;INT ;OFF ;INC 1.5
    and
    xM:MODF? = :FM:MODF;VALUE 5000.0;SIN ;INC 1000.0 */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_modulationQuery( ViSession vi,
                                          ViInt16  channel,
                                          ViReal64 *depth,        /* or deviation */
                                          ViReal64 *depthInc,
                                          ViReal64 *freq,
                                          ViReal64 *freqInc,
                                          ViInt16  *source,
                                          ViInt16  *shape,
                                          ViInt16  *state )
{
  ViStatus status = VI_SUCCESS;
  char    buffer[128];
  ViChar  b1[128];
  ViChar  b2[128];

  strcpy(buffer, mod_channel[channel]);
  strcat(buffer, "?\n" );

  status = viPrintf( vi, buffer );
  if( status >= VI_SUCCESS )
    status = viScanf( vi,
                      "%*s %lf; %s ;%s %*s %lf %*t",
                      depth, b1, b2, depthInc );

  if( status >= VI_SUCCESS )
  {
    if( strcmp(b1, "INT") == 0 )
      *source = ifr2025_SRC_INT;
    else
    if( strcmp(b1, "EXTAC") == 0 )
      *source = ifr2025_SRC_EXTAC;
    else
    if( strcmp(b1, "EXTALC") == 0 )
      *source = ifr2025_SRC_EXTALC;
    else
    if( strcmp(b1, "EXTDC") == 0 )
      *source = ifr2025_SRC_EXTDC;
  
    if( strcmp(b2,"OFF" ) == 0 )
      *state = ifr2025_OFF;
    else
      *state = ifr2025_ON;
  }
  
  if( status >= VI_SUCCESS )
  {
    strcpy(buffer, mod_channel[channel]);
    strcat(buffer, ":MODF?\n" );

    status = viPrintf( vi, buffer );
    if( status >= VI_SUCCESS )
      status = viScanf( vi,
                      "%*s %lf; %s %*s %lf %*t",
                      freq, b1, freqInc );
                      
    if( status >= VI_SUCCESS )
    {
      if( strcmp(b1, "SIN") == 0 )
        *shape = ifr2025_MODF_SIN;
      else
      if( strcmp(b1, "TRI") == 0 )
        *shape = ifr2025_MODF_TRI;
      else
      if( strcmp(b1, "SQR") == 0 )
        *shape = ifr2025_MODF_SQR;
    }
  }
  
  RETURN( vi, status );
}

/*========================================================*/
/*                QUERY INSTRUMENT MODES                  */
/*                ======================                  */
/*  Get the responses from                                */
/*  MODE? = :MODE AM,FM,PULSE  (example)                  */
/*  MOD?  = :MOD:ON            (or OFF)                   */
/*  PULSE?= :PULSE:ON          (or OFF)                   */
/*                                                        */
/*========================================================*/
ViStatus _VI_FUNC ifr2025_modeQuery( ViSession vi,
                                    ViInt16 *mMode,
                                    ViInt16 *mPulse,
                                    ViInt16 *globalMod,
                                    ViInt16 *globalPulse )
{
  ViStatus  status;
  ViChar    buffer[256];
  
  status = viPrintf( vi, "MODE?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %t", buffer );
    
  if( status >= VI_SUCCESS )
  {
    ViInt16 modeX = 0;
    
    if( strstr(buffer, "AM" ) != NULL )
      modeX |= 1;
    if( strstr(buffer, "FM" ) != NULL )
      modeX |= 2;
    if( strstr(buffer, "PM" ) != NULL )
      modeX |= 4;
    if( strstr(buffer, "FSK2L" ) != NULL )
      modeX |= 8;
    if( strstr(buffer, "FSK4L" ) != NULL )
      modeX |= 16;

    switch( modeX )
    {
      case 1:
        *mMode = ifr2025_MODE_AM;
        break;
      case 2:
        *mMode = ifr2025_MODE_FM;
        break;
      case 3:
        *mMode = ifr2025_MODE_AMFM;
        break;
      case 4:
        *mMode = ifr2025_MODE_PM;
        break;
      case 5:
        *mMode = ifr2025_MODE_AMPM;
        break;
      case 8:
        *mMode = ifr2025_MODE_2FSK;
        break;
      case 16:
        *mMode = ifr2025_MODE_4FSK;
        break;
      default:
        *mMode = ifr2025_MODE_AM;
        status = VI_ERROR_PARAMETER2;
        break;
    }
    
    if( strstr( buffer, ",PULSE" ) != NULL )
      *mPulse = ifr2025_PULSE_ON;
    else
      *mPulse = ifr2025_PULSE_OFF;
  }
  
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "MOD?\n" );
    
  if( status >= VI_SUCCESS )
    status = viScanf( vi, ":MOD:%t", buffer );
    
  if( status >= VI_SUCCESS )
    if( strncmp( buffer, "ON", 2 ) == 0 )
      *globalMod = ifr2025_ON;
    else
      *globalMod = ifr2025_OFF;

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "PULSE?\n" );
    
  if( status >= VI_SUCCESS )
    status = viScanf( vi, ":PULSE:%t", buffer );
    
  if( status >= VI_SUCCESS )
    if( strncmp( buffer, "ON", 2 ) == 0 )
      *globalPulse = ifr2025_ON;
    else
      *globalPulse = ifr2025_OFF;

  RETURN( vi, status );
}

/*----------- END INSTRUMENT-DEPENDENT ROUTINES HERE -------------------*/

/* ==================================================================== */
/*   Marconi GPIB - 2024 extensions                                     */
/* ==================================================================== */
/*=========================================================================*/
/* Set Sum of Depth of Modulation, depth is in %                           */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_depthSDM( ViSession vi, ViReal64 depth )
{
  ViStatus status;

  status = CHECKVAL(depth, ifr2025_DEPTH_SDM_MIN, ifr2025_DEPTH_SDM_MAX, ifr2025_ERROR_DEPTH );
  
  if( status > VI_SUCCESS )
    status = viPrintf(vi, "SDM %f\n", depth );
  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set Difference in Depth of Modulation for 90 or 150 Hz predominant      */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_depthDDM( ViSession vi, ViInt16 type, ViReal64 depth )
{
  ViStatus status;

  status = CHECKVAL(depth, ifr2025_DEPTH_DDM_MIN, ifr2025_DEPTH_DDM_MAX, ifr2025_ERROR_DEPTH );
  
  if( status > VI_SUCCESS )
  {
    switch( type )
    {
      case ifr2025_DDM90:
        status = viPrintf(vi, "DDM90 %f PCT\n", depth );
        break;
        
      case ifr2025_DDM150:
        status = viPrintf(vi, "DDM150 %f PCT\n", depth );
        break;
      
      default:
        status = ifr2025_ERROR_DDM_TYPE;
    }
  }
  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set ILD Ident Tone Depth                                                */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_depthILSID( ViSession vi, ViReal64 depth )
{
  ViStatus status;

  status = CHECKVAL(depth, ifr2025_DEPTH_ILS_MIN, ifr2025_DEPTH_ILS_MAX, ifr2025_ERROR_DEPTH );
  
  if( status > VI_SUCCESS )
    status = viPrintf(vi, "ILSID %f PCT\n", depth );
  
  RETURN( vi, status );
}

/*========================================================*/
/*                QUERY ILS SETUP                         */
/*                ===============                         */
/*  Get the responses from                                */
/*  SDM?  = :SDM:DEPTH 12.0;INC 3.3                       */
/*  DDM?  = :DDM90:DEPTH 30.2;INC 1.0                     */
/*  ILSID?= :ILSID:DEPTH 23.3;INC 2.2                     */
/*                                                        */
/*========================================================*/
ViStatus _VI_FUNC ifr2025_ilsQuery(  ViSession vi,
                                    ViReal64  *depthSDM,
                                    ViReal64  *depthDDM,
                                    ViReal64  *depthILSID,
                                    ViInt16   *ddmType )
{
  ViStatus  status;
  ViChar    buffer[256];
  
  status = viPrintf( vi, "SDM?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %f %*t", depthSDM );
    

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "DDM?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%s %f %*t", buffer, depthDDM );

  if( status >= VI_SUCCESS )
  {
    if( strstr( buffer, ":DDM90:" ) != NULL )
      *ddmType = ifr2025_DDM90;
    else
      *ddmType = ifr2025_DDM150;
  }
  
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "ILSID?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %f %*t", depthSDM );

  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set VOR SUBcarrier (9960Hz) Depth in %                                  */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_depthVOR( ViSession vi, ViReal64 depth )
{
  ViStatus status;
  
  status = CHECKVAL(depth, ifr2025_DEPTH_VOR_MIN, ifr2025_DEPTH_VOR_MAX, ifr2025_ERROR_DEPTH );
  
  if( status > VI_SUCCESS )
    status = viPrintf(vi, "VOR %fPCT\n", depth );
  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set REF Variable signal (30Hz) depth in %                               */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_depthREF( ViSession vi, ViReal64 depth )
{
  ViStatus status;
  
  status = CHECKVAL(depth, ifr2025_DEPTH_REF_MIN, ifr2025_DEPTH_REF_MAX, ifr2025_ERROR_DEPTH );
  
  if( status > VI_SUCCESS )
    status = viPrintf(vi, "REF %fPCT\n", depth );
  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set VOR Bearing to/from the beacon in degrees                           */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_bearingVOR( ViSession vi, ViInt16 direction, ViReal64 bearing )
{
  ViStatus status;
  
  status = CHECKVAL( bearing, ifr2025_BEARING_MIN, ifr2025_BEARING_MAX, ifr2025_ERROR_DEPTH );

  if( status > VI_SUCCESS )
  {
    switch( direction )
    {
      case ifr2025_TO:
        status = viPrintf(vi, "BEARTO %f\n", bearing );
        break;
        
      case ifr2025_FROM:
        status = viPrintf(vi, "BEARFR %f\n", bearing );
        break;
        
      default:
        status = ifr2025_ERROR_BEARING;
        break;
    }
  }
  
  RETURN( vi, status );
}

/*=========================================================================*/
/* Set VOR Ident tone depth in %                                           */
/*=========================================================================*/
ViStatus _VI_FUNC ifr2025_depthVORID( ViSession vi, ViReal64 depth )
{
  ViStatus status;
  
  status = CHECKVAL(depth, ifr2025_DEPTH_REF_MIN, ifr2025_DEPTH_REF_MAX, ifr2025_ERROR_DEPTH );
  
  if( status > VI_SUCCESS )
    status = viPrintf(vi, "VORID %fPCT\n", depth );
  
  RETURN( vi, status );
}

/*========================================================*/
/*                QUERY VOR SETUP                         */
/*                ===============                         */
/*  Get the responses from (for example)                  */
/*  VOR?  = :SUB:DEPTH 12.0;INC 3.3                       */
/*  REF?  = :REF:DEPTH 30.2;INC 1.0                       */
/*  BEAR? = :BEARFR:VALUE 300.0;INC 0.5                   */
/*  VORID?= :VORID:DEPTH 30.2;INC 1.0                     */
/*                                                        */
/*========================================================*/
ViStatus _VI_FUNC ifr2025_vorQuery(  ViSession vi,
                                    ViReal64  *depthVOR,
                                    ViReal64  *depthREF,
                                    ViReal64  *depthVORID,
                                    ViInt16   *direction,
                                    ViReal64  *bearing )
{
  ViStatus  status;
  ViChar    buffer[256];
  
  status = viPrintf( vi, "VOR?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %f %*t", depthVOR );
    

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "REF?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%s %f %*t", buffer, depthREF );

  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "VORID?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %f %*t", depthVORID );

  
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "ILSID?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%s %f %*t", buffer, bearing );

  if( status >= VI_SUCCESS )
  {
    if( strstr( buffer, ":BEARTO" ) != NULL )
      *direction = ifr2025_TO;
    else
      *direction = ifr2025_FROM;
  }

  
  RETURN( vi, status );
}


/* ==================================================================== */
/*  2023A/23B/25 extensions                                             */
/* ==================================================================== */
/*========================================================================*/
/* Specify Sinad parameters                                                   */
/*========================================================================*/

ViStatus _VI_FUNC ifr2025_sinad (ViSession vi, int averages, ViInt16 filterWeighting,
                                ViInt16 mode, ViReal64 autoTarget,
                                ViReal64 deltaLevelStep)
{                                
  ViStatus status = VI_SUCCESS;

  status = CHECKVAL(averages, 1, 127, ifr2025_ERROR_SINADAV );
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "SINAD:AVERAGE %d\n", averages);
  
  if( status >= VI_SUCCESS )
    switch( filterWeighting )
    {
      case 0:
        status = viPrintf( vi, "SINAD:UNWTD\n");
        break;
        
      case 1:
        status = viPrintf( vi, "SINAD:CCITT\n");
        break;
        
      case 2:
        status = viPrintf( vi, "SINAD:CMESS\n");
        break;
        
      default:
        status = VI_ERROR_PARAMETER3;
        break;
    }
  
  if( status >= VI_SUCCESS )
    switch( mode )
    {
      case 0:
        status = viPrintf( vi, "SINAD:DISABLE\n");
        break;
        
      case 1:
        status = viPrintf( vi, "SINAD:ENABLE\n");
        break;
        
      case 2:
        status = viPrintf( vi, "SINAD:AUTO:VALUE %lf\n", autoTarget);
        break;
        
      default:
        status = VI_ERROR_PARAMETER4;
        break;
    }
  
  if( status >= VI_SUCCESS )
    status = viPrintf( vi, "RFLV:INC %lf\n", deltaLevelStep);
  
  RETURN( vi, status );
}
/*========================================================================*/
/* Start/Stop automatic Sinad levelling process                           */
/*========================================================================*/

ViStatus _VI_FUNC ifr2025_autoSinad (ViSession vi, ViInt16 state)
{
  ViStatus status = VI_SUCCESS;

  switch( state )
  {
    case 0:
      status = viPrintf( vi, "SINAD:AUTO:PAUSE\n");
      break;
        
    case 1:
      status = viPrintf( vi, "SINAD:AUTO:SEEK\n");
      break;
        
    default:
      status = VI_ERROR_PARAMETER2;
      break;
  }
  
  RETURN( vi, status );
}
/*========================================================================*/
/* Query Sinad setup parameters                                           */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_sinadSetupQuery (ViSession vi, ViPInt16 averages,
                                          ViPInt16 filterWeighting, ViPInt16 mode,
                                          ViPReal64 targetSinad,
                                          ViPReal64 deltaLevelStep)
{
  ViStatus status = VI_SUCCESS;
  ViChar fi[32], mo[32];

  status = viPrintf (vi, "SINAD?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %d %*c %s %*c %s %*t", averages, fi, mo );
    
  if( status >= VI_SUCCESS )
  {
    if( strcmp(fi, "UNWTD") == 0 )
      *filterWeighting = ifr2025_NONE;
    else
    if( strcmp(fi, "CCITT") == 0 )
      *filterWeighting = ifr2025_CCITT;
    else
    if( strcmp(fi, "CMESS") == 0 )
      *filterWeighting = ifr2025_CMESS;
    else
      *filterWeighting = ifr2025_NONE;

    if( strcmp(mo, "DISABLE") == 0 )
      *mode = ifr2025_DISABLE;
    else
    if( strcmp(mo,"ENABLE" ) == 0 )
      *mode = ifr2025_ENABLE;
    else
    if( strcmp(mo,"AUTO" ) == 0 )
    {
      *mode = ifr2025_AUTO;
      if( status >= VI_SUCCESS )
        status = viPrintf (vi, "SINAD:AUTO:VALUE?\n" );
  
      if( status >= VI_SUCCESS )
        status = viScanf( vi, "%lf %*t", targetSinad );
    }
    else
      *mode = ifr2025_DISABLE;
  }
  
  if( status >= VI_SUCCESS )
    status = viPrintf (vi, "RFLV?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%*s %*s %*s %*s %lf %*s %*t", deltaLevelStep );
  
  RETURN( vi, status );
}
/*========================================================================*/
/* Query current measured Sinad value                                     */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_sinadReadyQuery (ViSession vi, ViPInt16 statusCode)
{
  ViStatus status = VI_SUCCESS;
  ViInt16 statusReg;
  
  *statusCode = ifr2025_SINAD_NONE;

  status = viPrintf (vi, "SSR?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%d %*t", &statusReg );

  if( status >= VI_SUCCESS )
    if( (statusReg & 0x20) == 0x20)
      *statusCode = ifr2025_SINAD_READY;
      
  RETURN( vi, status );
}
/*========================================================================*/
/* Query current measured Sinad value                                     */
/*========================================================================*/
ViStatus _VI_FUNC ifr2025_sinadValueQuery (ViSession vi, ViPReal64 sinadValue)
{
  ViStatus status = VI_SUCCESS;

  status = viPrintf (vi, "SINAD:VALUE?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%lf %*t", sinadValue );

  RETURN( vi, status );
}
/*========================================================================*/
/* Query automatic Sinad process status                                   */
/*========================================================================*/

ViStatus _VI_FUNC ifr2025_autoSinadQuery (ViSession vi, ViPInt16 statusCode)
{                                         
  ViStatus status = VI_SUCCESS;
  ViInt16 eventReg;
  
  *statusCode = ifr2025_SINAD_IDLE;

  status = viPrintf (vi, "SCR?\n" );
  
  if( status >= VI_SUCCESS )
    status = viScanf( vi, "%d %*t", &eventReg );

  if( status >= VI_SUCCESS )
    if( (eventReg & 0x10) == 0x10)
      *statusCode = ifr2025_SINAD_ACTIVE;
      
  RETURN( vi, status );
}

/*====== UTILITY ROUTINES =================================================*/


/*=== THE END =============================================================*/
