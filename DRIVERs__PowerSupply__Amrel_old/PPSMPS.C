#include <utility.h>
#include <ansi_c.h>

#include <gpib.h>
#include <formatio.h>
#include "ppsmps.h"

/*= Amrel PPS-MPS Series DC Power Supplies Instrument Driver ==============*/
/* LabWindows Source Code                                                  */
/* By: Innovative Test & Programming                                       */
/* Modification History: 					                               */
/*  Rev:	Who:			Date:		Description:					   */
/*  1.0		M.W.O'Brien     08/16/95	Initial Release					   */
/*=========================================================================*/

/*= INSTRUMENT TABLE ======================================================*/
/* address array: contains the GPIB addresses of opened instruments.       */
/* bd array: contains the device descriptors returned by OpenDev.          */
/* instr_cnt: contains the number of instruments open of this model type.  */
/* ppsmps_err: the error variable for the instrument module                */
/*=========================================================================*/
static int address[PPSMPS_MAX_INSTR + 1];
static int bd[PPSMPS_MAX_INSTR + 1];
static int instr_cnt;
static int ppsmps_err;
static char *ppsmps_range_error[16]; /* Function argument range error strings */

/*= INTERNAL DATA =========================================================*/
/* cmd is a buffer for GPIB I/O strings */
static char cmd[50];
static char device_id[10];    /* GPIB table device ID setting */
static int instrument_id;       /* instrument handle that points to a device */

/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS ===================================*/
/* INSERT COMMAND ARRAYS OF YOUR INSTRUMENT HERE. SEE THE BOTTOM OF THIS   */
/* FILE FOR FURTHER EXPLANATION.                                           */
/*=========================================================================*/

/*= UTILITY ROUTINES ======================================================*/
int ppsmps_open_instr (int addr);
int ppsmps_close_instr (int instrID);
int ppsmps_invalid_short_range (short val, short min, short max, int err_code);
int ppsmps_invalid_integer_range (int val, int min, int max, int err_code);
int ppsmps_invalid_longint_range (long val, long min, long max, int err_code);
int ppsmps_invalid_real_range (double val, double min, double max, int err_code);
int ppsmps_device_closed (int instrID);
int ppsmps_read_data (int instrID, char *buf, long cnt);
int ppsmps_write_with_status(int instrument_id, char *buf, long cnt);       /* return PS status error info */
int ppsmps_write_data (int instrID, char *buf, long cnt);
int ppsmps_read_data_file (int instrID, char *filename);
int ppsmps_write_data_file (int instrID, char *filename);
int ppsmps_poll (int instrID, char *response);
int ppsmps_set_timeout (int instrID, int tmo_code);
void ppsmps_setup_arrays (void);


/*----------- INSERT INSTRUMENT-DEPENDENT ROUTINES HERE -------------------*/

/*=========================================================================*/
/* Function: initialize                                                    */
/* Purpose:  This function opens the instrument, queries the instrument    */
/*           for its ID, and initializes the instrument to a known state.  */
/*=========================================================================*/

int ppsmps_initialize( address, unit, resetps, instrID , id_query)
  int address;
  int unit;
  int resetps;
  int *instrID;
  {
  int ID;
  short temp;
  int channel, mode;
  char tempstr[128];

    if (ppsmps_invalid_integer_range (address, 0, 30,  -12) != 0)
        return ppsmps_err;
    if (ppsmps_invalid_integer_range (unit, 0, 1,  -8) != 0)
        return ppsmps_err;
    if (ppsmps_invalid_integer_range (resetps, 0, 1,  -11) != 0)
        return ppsmps_err;

    /* Setup driver specific arrays */
    ppsmps_setup_arrays ();

    ID = ppsmps_open_instr (address);
    if (ID <= 0)
        return ppsmps_err;

    if (resetps)  /* reset PS to default setting */
      {
		if (unit)
		  {
		    /*Dual channel unit PS */
  			if (ppsmps_set_ch_on_off(ID, unit, 2, 0) !=0)  /* CHs off */
  	  		return  ppsmps_err;

		    if (ppsmps_set_ovp_enable (ID, unit, 2, 0) !=0)   /* disable OVP */
		    return  ppsmps_err;

    	    if (ppsmps_set_ocp_enable (ID, unit, 2, 0) !=0)  /* disable OCP */
  	  		return  ppsmps_err;

			if (ppsmps_set_voltage (ID, unit, 2, 0.0, 0.0) !=0)  /* CH1&2 0VDC */
      		return  ppsmps_err;

  			if (ppsmps_set_track (ID, 0) !=0)   /* disable track mode */
  	  		return  ppsmps_err;
		  }

		else
		  {
		    /* Single channel PS */
	        if (ppsmps_set_ch_on_off(ID, unit,0, 0) !=0)  /* CH off */
  	  		return  ppsmps_err;

		    if (ppsmps_set_ovp_enable (ID, unit, 0, 0) !=0)   /* disable OVP */
		    return  ppsmps_err;

    	    if (ppsmps_set_ocp_enable (ID, unit, 0, 0) !=0)  /* disable OCP */
  	  		return  ppsmps_err;

			if (ppsmps_set_voltage (ID, unit, 0, 0.0, 0.0) !=0)  /* CH1 0VDC */
      		return  ppsmps_err;
		  }
      }

    ppsmps_err = ppsmps_query_error_state (ID, &ppsmps_err, tempstr);
    if (ppsmps_err != 0) {
        ppsmps_close_instr (ID);
        return ppsmps_err;
     }

  *instrID = ID;
  return ppsmps_err;  /* return error */
 }

/*=========================================================================*/
/* Function: ppsmps_get_arg_range_error_msg                                */
/* Purpose:  This function returns an error description string for 5000    */
/*           series error codes. In the context of this driver 5000 series */
/*           error codes refer to function call argument ranges.           */
/*=========================================================================*/
void ppsmps_get_arg_range_error_msg (error_num, error_msg)
  int error_num;
  char *error_msg;
  {
  /* Get the error number into the proper range for indexing the message array */

  if (error_num == 0)
	{
	Fmt(error_msg,"%s","No Errors");
	}
  else
    {
    error_num = abs(error_num);
	error_num -= 1;
    error_num -= 5000; /* Convert to zero based */

    /* Return the error message */
    if ((error_num < 0)||(error_num > PPSMPS_MAX_RANGE_ERROR_NUM))
      Fmt(error_msg,"%s","Undefined Error!");
    else
      Fmt(error_msg,"%s", ppsmps_range_error[error_num]);
    }
  }

/*=========================================================================*/
/* Function: measure                                                       */
/* Purpose:  This function measures the voltage or current level	       */
/*              														   */
/*=========================================================================*/

int ppsmps_measure (instrID, unit, channel, meastype, result)
int instrID;
int unit;
int channel;
int meastype;
double *result;
{
char type[28]={0};
char buf[512]={0};

 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (channel, 0, 1,  -5003) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (meastype, 0, 1,  -5004) != 0)
     return ppsmps_err;

 if (meastype == 0)
   Fmt (type, "VOUT");
 else
   Fmt (type, "IOUT");


 if (unit == PPSMPS_SINGLE)  /* single channel unit */
    {
     Fmt (cmd, "%s?", type);
    }
 else
    {
     switch (channel)  /* dual channel unit */
  	   {
       case PPSMPS_CHAN1:   /* channel 1 selected */
         	Fmt (cmd, "%s1?", type);
        	break;
       case PPSMPS_CHAN2:   /* channel 2 selected */
            Fmt (cmd, "%s2?", type);
    	    break;
       default:
        	return 1;
     	    break;
       }
    }


   /* Issue query command */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */

   /* Issue query command */
   if (ppsmps_read_data (instrID, buf, 50) !=0)
 	  return ppsmps_err;   /* error occurred */

   Fmt(result, "%f<%s", buf);

   return 0;  /* OK */
}

/*=========================================================================*/
/* Function: query_error_state                                             */
/* Purpose:  This function queries the instrument for error status         */
/*              														   */
/*=========================================================================*/

int ppsmps_query_error_state (instrID, code, estring)
int instrID;
int *code;
char estring[128];   /* string are always passed by reference */
{
char buf[512]={0};

 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

   Fmt (cmd, "ERROR?");

   /* Issue query command */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */


   /* Read response */

   if (ppsmps_read_data (instrID, buf, 50) !=0)
 	  return ppsmps_err;   /* error occurred */

   Fmt(code, "%d<%s", buf);


   switch(*code)
     {
      case 0:
        Fmt(estring, PPSMPS_ERROR_0);	     /* no error */
        *code = 0;
        break;
      case 1:
        Fmt(estring,PPSMPS_ERROR_1);	    /* Command String Error */
        *code = -1;
        break;
      case 2:
        Fmt(estring,PPSMPS_ERROR_2);	    /*  Numeric string out of range */
        *code = -2;
        break;
      case 3:
        Fmt(estring,PPSMPS_ERROR_3);	    /* Numeric string over length */
        *code = -3;
        break;
      case 4:
        Fmt(estring,PPSMPS_ERROR_4);		/* Command sequence error */
        *code = -4;
        break;
      case 5:
        Fmt(estring,PPSMPS_ERROR_5);		/* Undefined */
        *code = -5;
        break;
      case 6:
        Fmt(estring,PPSMPS_ERROR_6);		/* Undefined */
        *code = -6;
        break;
     }
ppsmps_err = *code;      /* set return code equal to error query result */
return(ppsmps_err);     /* return OK or error code */
}

/*=========================================================================*/
/* Function: query_model_number                                            */
/* Purpose:  This function queries the supply for it's model number	       */
/*              														   */
/*=========================================================================*/

int ppsmps_query_model_number (instrID, model)
int instrID;
char *model;
{

 int cr_position;
 int spaceloc;
 char temp[32];

 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

   /* Clear the model string */
   Fmt(model,"");

   Fmt (cmd, "MODEL?");

   /* Issue query command */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */

   /* Read response */
   if (ppsmps_read_data (instrID, model, 31) !=0)
 	  return ppsmps_err;   /* error occurred */

   /* Strip any leading spaces from the string returned */
   spaceloc = FindPattern (model, 0, 1, " ", 0, 0);
   for(;spaceloc >= 0;)
     {
	 CopyBytes (model, 0, model, spaceloc + 1, StringLength(model));
     spaceloc = FindPattern (model, 0, 1, " ", 0, 0);
     }

 return(0); /* OK */
}

/*=========================================================================*/
/* Function: query_set_curr_volt                                           */
/* Purpose:  This function returns the programmed Over Voltage Protection  */
/*           level for the specified channel1.
/*=========================================================================*/
int ppsmps_query_over_voltage_levels (instrID, unit, channel, ovp_value)
  int instrID;
  int unit;
  int channel;
  double *ovp_value;
  {
  char buf[512]={0};


  if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
     return ppsmps_err;

  if (ppsmps_device_closed (instrID) != 0)
      return ppsmps_err;

  if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
      return ppsmps_err;

  if (ppsmps_invalid_integer_range (channel, 0, 1,  -5003) != 0)
      return ppsmps_err;

  /* Build command string */
  Fmt(cmd,"OVSET");

  if (unit == PPSMPS_SINGLE)
    Fmt (cmd, "%s?", cmd); /* Single channel unit */
  else
    {
   	switch (channel)  /* Dual channel unit */
  	  {
      case PPSMPS_CHAN1:   /* Channel 1 selected */
       	Fmt (cmd, "%s1?", cmd);
        break;
      case PPSMPS_CHAN2:   /* Channel 2 selected */
        Fmt (cmd, "%s2?", cmd);
        break;
      }
    }

  /* Issue query command */
  if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
    return ppsmps_err;  /* error occurred */

  /* Issue query command */
  if (ppsmps_read_data (instrID, buf, 50) !=0)
    return ppsmps_err;   /* error occurred */

  Fmt(ovp_value, "%f<%s", buf);

  return 0;  /* OK */

  }


/*=========================================================================*/
/* Function: query_set_curr_volt                                           */
/* Purpose:  This function returns the set voltage or current level	       */
/*           Note: Returns the programmed values not the actual output     */
/*				   levels.												   */
/*=========================================================================*/

int ppsmps_query_set_curr_volt (instrID, unit, channel, parameter, value)
int instrID;
int unit;
int channel;
int parameter;
double *value;
{
char type[5];
char buf[512]={0};

 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (parameter, 0, 1,  -5004) != 0)
     return ppsmps_err;

 if (parameter == 0)
   Fmt (type, "VSET");
 else
   Fmt (type, "ISET");


 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "%s?", type);
   }
 else
   {
  	switch (channel)  /* dual channel unit */
  	   {
       case PPSMPS_CHAN1:   /* channel 1 selected */
         	Fmt (cmd, "%s1?", type);
        	break;
       case PPSMPS_CHAN2:   /* channel 2 selected */
            Fmt (cmd, "%s2?", type);
    	    break;
       default:
        	return 1;
     	    break;
       }
   }

   /* Issue query command */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */

   /* Issue query command */
   if (ppsmps_read_data (instrID, buf, 50) !=0)
 	  return ppsmps_err;   /* error occurred */



   Fmt(value, "%f<%s", buf);

   return 0;  /* OK */
}


/*=========================================================================*/
/* Function: set_voltage                                                   */
/* Purpose:  This function sets the output voltage level			       */
/*              														   */
/*=========================================================================*/

int ppsmps_set_voltage (instrID, unit, channel, voltage1, voltage2)
int instrID;
int unit;
int channel;
double voltage1;
double voltage2;
{
    if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
        return ppsmps_err;

    if (ppsmps_device_closed (instrID) != 0)
        return ppsmps_err;

    if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
        return ppsmps_err;

    if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
        return ppsmps_err;

 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "VSET %f[p3]", voltage1);
    if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
      return ppsmps_err;   /* error occurred */
   }
  else
   {
	switch (channel)  /* dual channel unit */
	   {
	    case PPSMPS_CHAN1:   /* channel 1 selected */
	       	Fmt (cmd, "VSET1 %f[p3]", voltage1);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN2:   /* channel 2 selected */
	        Fmt (cmd, "VSET2 %f[p3]", voltage2);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN_BOTH:   /* both selected */
	    	Fmt (cmd, "VSET1 %f[p3]", voltage1);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */

			Delay(PPSMPS_CMD_DELAY);

	    	Fmt (cmd, "VSET2 %f[p3]", voltage2);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    default:
	    	break;
	   }
   }

return 0; /* return OK */
}

/*=========================================================================*/
/* Function: set_ovp                                                       */
/* Purpose:  This function will setup the overvoltage limits for the       */
/*           power supply.												   */
/*=========================================================================*/

int ppsmps_set_ovp (instrID, unit, channel, ovp1, ovp2)
int instrID;
int unit;
int channel;
double ovp1;
double ovp2;
{
    if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
        return ppsmps_err;

    if (ppsmps_device_closed (instrID) != 0)
        return ppsmps_err;

    if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
        return ppsmps_err;

    if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
        return ppsmps_err;

 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "OVSET %f[p3]", ovp1);
    if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
      return ppsmps_err;   /* error occurred */
   }
 else
   {
	switch (channel)  /* dual channel unit */
	   {
	    case PPSMPS_CHAN1:   /* channel 1 selected */
	       	Fmt (cmd, "OVSET1 %f[p3]", ovp1);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN2:   /* channel 2 selected */
	        Fmt (cmd, "OVSET2 %f[p3]", ovp2);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN_BOTH:   /* both selected */
	    	Fmt (cmd, "OVSET1 %f[p3]", ovp1);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */

			Delay(PPSMPS_CMD_DELAY);

	    	Fmt (cmd, "OVSET2 %f[p3]", ovp2);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    default:
	    	break;
	   }
   }

 return 0; /* return OK */
}
/*=========================================================================*/
/* Function: set_current                                                   */
/* Purpose:  This function will setup the current limits for the           */
/*           power supply.												   */
/*=========================================================================*/

int ppsmps_set_current (instrID, unit, channel, current1, current2)
int instrID;
int unit;
int channel;
double current1;
double current2;
{
    if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
        return ppsmps_err;

    if (ppsmps_device_closed (instrID) != 0)
        return ppsmps_err;

    if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
        return ppsmps_err;

    if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
        return ppsmps_err;

 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "ISET %f[p3]", current1);
    if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
      return ppsmps_err;   /* error occurred */
   }
  else
   {
	switch (channel)  /* dual channel unit */
	   {
	    case PPSMPS_CHAN1:   /* channel 1 selected */
	       	Fmt (cmd, "ISET1 %f[p3]", current1);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN2:   /* channel 2 selected */
	        Fmt (cmd, "ISET2 %f[p3]", current2);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN_BOTH:   /* both selected */
	    	Fmt (cmd, "ISET1 %f[p3]", current1);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */

			Delay(PPSMPS_CMD_DELAY);

	    	Fmt (cmd, "ISET2 %f[p3]", current2);
            if (ppsmps_write_with_status(instrID, cmd, NumFmtdBytes ()) != 0)
              return ppsmps_err;   /* error occurred */
              break;
	    default:
	    	break;
	   }
   }

  return 0; /* return OK */
}

/*=========================================================================*/
/* Function: set_ovp_enable                                                */
/* Purpose:  This function will enable or disable the OVP circuitry of the */
/*           power supply.												   */
/*=========================================================================*/

int ppsmps_set_ovp_enable (instrID, unit, channel, mode)
int instrID;
int unit;
int channel;
int mode;
{
 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (mode, 0, 1,  -5004) != 0)
     return ppsmps_err;

 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "OVP %d", mode);
    if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */
   }
  else
   {
	switch (channel)  /* dual channel unit */
	   {
	    case PPSMPS_CHAN1:   /* channel 1 selected */
	       	Fmt (cmd, "OVP1 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN2:   /* channel 2 selected */
	        Fmt (cmd, "OVP2 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN_BOTH:   /* both selected */
	    	Fmt (cmd, "OVP1 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */

			Delay(PPSMPS_CMD_DELAY);

	    	Fmt (cmd, "OVP2 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
            break;
	    default:
	    	break;
	   }
   }

 return 0; /* return OK */
}

/*=========================================================================*/
/* Function: set_ocp_enable                                                */
/* Purpose:  This function will enable or disable the OCP circuitry of the */
/*           power supply.												   */
/*=========================================================================*/

int ppsmps_set_ocp_enable (instrID, unit, channel, mode)
int instrID;
int unit;
int channel;
int mode;
{
 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (mode, 0, 1,  -5004) != 0)
     return ppsmps_err;

 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "OCP %d", mode);
    if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */
   }
  else
   {
	switch (channel)  /* dual channel unit */
	   {
	    case PPSMPS_CHAN1:   /* channel 1 selected */
	       	Fmt (cmd, "OCP1 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN2:   /* channel 2 selected */
	        Fmt (cmd, "OCP2 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN_BOTH:   /* both selected */
	    	Fmt (cmd, "OCP1 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */

			Delay(PPSMPS_CMD_DELAY);

	    	Fmt (cmd, "OCP2 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    default:
	    	break;
	   }
   }

return 0; /* return OK */
}

/*=========================================================================*/
/* Function: set_ch_on_off                                                 */
/* Purpose:  This function will turn ON & OFF the output relays on the     */
/*           power supply.												   */
/*=========================================================================*/

int ppsmps_set_ch_on_off(instrID, unit, channel, mode)
int instrID;
int unit;
int channel;
int mode;
{
 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (mode, 0, 1,  -5004) != 0)
     return ppsmps_err;

 if (unit == PPSMPS_SINGLE)  /* single channel unit */
   {
    Fmt (cmd, "OUT %d", mode);
    if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */
   }
  else
   {
	switch (channel)  /* dual channel unit */
	   {
	    case PPSMPS_CHAN1:   /* channel 1 selected */
	       	Fmt (cmd, "OUT1 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN2:   /* channel 2 selected */
	        Fmt (cmd, "OUT2 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    case PPSMPS_CHAN_BOTH:   /* both selected */
	    	Fmt (cmd, "OUT1 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */

			Delay(PPSMPS_CMD_DELAY);

	    	Fmt (cmd, "OUT2 %d", mode);
            if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
              return ppsmps_err;   /* error occurred */
	    	break;
	    default:
	    	break;
	   }
   }

return 0; /* return OK */
}

/*=========================================================================*/
/* Function: set_address                                                   */
/* Purpose:  This function will set the address of the power supply        */
/*           Legal range for the address is  from 0 to 30.  			   */
/*=========================================================================*/

int ppsmps_set_address (instrID, new_address)
int instrID;
int new_address;
{
  if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (new_address, 0, 30,  -5002) != 0)
     return ppsmps_err;

 Fmt (cmd, "ADDRESS %d", new_address);

   /*  send command string  */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */

 return 0; /* return OK */
}

/*=========================================================================*/
/* Function: set_port                                                      */
/* Purpose:  This function will set the port of the power supply           */
/*           Legal range for the port is  from 0 to 127.      			   */
/*=========================================================================*/

int ppsmps_set_port (instrID, port)
int instrID;
int port;
{
  if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (port, 0, 127,  -5002) != 0)
     return ppsmps_err;

 Fmt (cmd, "PORT %d", port);

   /*  send command string  */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */

 return 0; /* return OK */
}


/*=========================================================================*/
/* Function: set_track                                                     */
/* Purpose:  This function will set the track mode of the power supply     */
/*           This mode is only valid for dual supply units. When set, the  */
/*           output of channel two will track the output setting of channel*/
/* 			 one.														   */
/*=========================================================================*/

int ppsmps_set_track (instrID, track)
int instrID;
int track;
{
 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (track, 0, 1,  -5002) != 0)
     return ppsmps_err;


 Fmt (cmd, "TRACK%d", track);

   /*  send command string  */
   if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
      return ppsmps_err;   /* error occurred */

 return 0; /* return OK */
}


/*=========================================================================*/
/* Function: ppsmps_config_single_chan_all                                        */
/* Purpose:  This function allows the user to set all parameters for a     */
/*           single channel power supply.    							   */
/*=========================================================================*/
int ppsmps_config_single_chan_all(instrID, ovp, ovp_enable, ocp_enable, voltage,
						          current, out_enable)
int instrID;
double ovp;
int ovp_enable;
int ocp_enable;
double voltage;
double current;
int out_enable;
{

  if (ppsmps_set_ovp (instrID, 0, 0, ovp, 0.0) != 0)
   	  return  ppsmps_err;

  if (ppsmps_set_ovp_enable (instrID, 0, 0, ovp_enable) !=0)
	  return  ppsmps_err;

  if (ppsmps_set_current (instrID, 0, 0, current, 0.0) !=0)
  	  return  ppsmps_err;

  if (ppsmps_set_ocp_enable (instrID, 0, 0, ocp_enable) !=0)
  	  return  ppsmps_err;

  if (ppsmps_set_voltage (instrID, 0, 0, voltage, 0.0) !=0)
      return  ppsmps_err;

  if (ppsmps_set_ch_on_off(instrID, 0, 0, out_enable) !=0)
  	  return  ppsmps_err;

 return 0;  /* OK */
}

/*=========================================================================*/
/* Function: configure_dual_CH_all                                         */
/* Purpose:  This function will set all parameters for a dual channel      */
/*           power supply.      										   */
/*=========================================================================*/
int ppsmps_configure_dual_CH_all(int instrID, double ovp1, double ovp2,
            int ovp1_enable, int ocp1_enable, int ovp2_enable, int ocp2_enable,
            double voltage1, double current1, double voltage2, double current2,
            int out1_enable, int out2_enable, int track, int channel)
  {
  int unit = 1; /* dual unit */
  int ch1 = 0;
  int ch2 = 1;
  int both = 2;

  if (ppsmps_set_track (instrID, track) !=0)
  	  return  ppsmps_err;

  if (ppsmps_set_ovp (instrID, unit, channel, ovp1, ovp2) != 0)
      return  ppsmps_err;

  switch (channel)
    {
    case 0:
      if (ppsmps_set_ovp_enable (instrID, unit, ch1, ovp1_enable) !=0)
          return  ppsmps_err;
	  break;
    case 1:
      if (ppsmps_set_ovp_enable (instrID, unit, ch2, ovp2_enable) !=0)
   	      return  ppsmps_err;
	  break;
	case 2:
      if (ppsmps_set_ovp_enable (instrID, unit, ch1, ovp1_enable) !=0)
          return  ppsmps_err;
      if (ppsmps_set_ovp_enable (instrID, unit, ch2, ovp2_enable) !=0)
   	      return  ppsmps_err;
	  break;
	}

  if (ppsmps_set_current (instrID, unit, channel, current1, current2) !=0)
  	  return  ppsmps_err;

  switch (channel)
    {
    case 0:
      if (ppsmps_set_ocp_enable (instrID, unit, ch1, ocp1_enable) !=0)
  	      return  ppsmps_err;
	  break;
    case 1:
      if (ppsmps_set_ocp_enable (instrID, unit, ch2, ocp2_enable) !=0)
   	      return  ppsmps_err;
	  break;
	case 2:
      if (ppsmps_set_ocp_enable (instrID, unit, ch1, ocp1_enable) !=0)
          return  ppsmps_err;
      if (ppsmps_set_ocp_enable (instrID, unit, ch2, ocp2_enable) !=0)
   	      return  ppsmps_err;
	  break;
	}

  if (ppsmps_set_voltage (instrID, unit, channel, voltage1, voltage2) !=0)
      return  ppsmps_err;

  switch (channel)
    {
    case 0:
      if (ppsmps_set_ch_on_off(instrID, unit, ch1, out1_enable) !=0)
  	      return  ppsmps_err;
	  break;
    case 1:
      if (ppsmps_set_ch_on_off(instrID, unit, ch2, out2_enable) !=0)
  	      return  ppsmps_err;
	  break;
	case 2:
      if (ppsmps_set_ch_on_off(instrID, unit, ch1, out1_enable) !=0)
  	      return  ppsmps_err;
      if (ppsmps_set_ch_on_off(instrID, unit, ch2, out2_enable) !=0)
  	      return  ppsmps_err;
	  break;
	}

  return 0;  /* OK */
  }

/*=========================================================================*/
/* Function: ppsmps_write_cal_data                                         */
/* Purpose:  This function allows the user to write calibration data to    */
/*           the power supply.											   */
/*=========================================================================*/
int ppsmps_write_cal_data(instrID, parameter, value)
int instrID;
int parameter;
double value;
{
 /* Verify valid ranges */
 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (parameter, 0, 3,  -5002) != 0)
    return ppsmps_err;

 /* Build the command string */
 switch (parameter)
   {
   case PPSMPS_VOLTAGE_OFFSET: 		/* Voltage Offset */
     Fmt(cmd, "VOFF ");
     break;
   case PPSMPS_FULL_SCALE_VOLTS: 	/* Full Scale Voltage */
     Fmt(cmd, "VFS ");
     break;
   case PPSMPS_CURRENT_OFFSET: 		/* Current Offset */
     Fmt(cmd, "IOFF ");
     break;
   case PPSMPS_FULL_SCALE_CURRENT: 	/* Full Scale Current */
     Fmt(cmd, "IFS ");
     break;
   }

 /* Add the parameter value to the command string */
 Fmt(cmd, "%s%f", cmd, value);

 if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
     return ppsmps_err;

 return 0;
}

/*=========================================================================*/
/* Function: enable_cal                                                    */
/* Purpose:  This function will set the calibration mode of the power      */
/*           supply. 													   */
/*=========================================================================*/

int ppsmps_enable_cal(instrID, unit, channel, mode)
int instrID;
int unit;
int channel;
int mode;
{
 char modelnum[128]; /* Model number buffer */

 /* Verify that all inputs are in range */
 if (ppsmps_invalid_integer_range (instrID, 1, PPSMPS_MAX_INSTR,  -5001) != 0)
    return ppsmps_err;

 if (ppsmps_device_closed (instrID) != 0)
     return ppsmps_err;

 if (ppsmps_invalid_integer_range (unit, 0, 1,  -5002) != 0)
    return ppsmps_err;

 if (ppsmps_invalid_integer_range (channel, 0, 2,  -5003) != 0)
    return ppsmps_err;

 if (ppsmps_invalid_integer_range (mode, 0, 1,  -5004) != 0)
    return ppsmps_err;

 /* Get the supply's model number */
 ppsmps_err = ppsmps_query_model_number (instrID, modelnum);
   if (ppsmps_err != 0)
     return ppsmps_err;

 /* Set the syntax for the CALCHNL command by power supply model.    */
 /* Some models require a space after the CALCHNL command and before */
 /* the argument. All of the supplies in the list below do not  	 */
 /* require the space.												 */
 if ((CompareStrings (modelnum, 0, "1322", 0, 0) == 0)||
	 (CompareStrings (modelnum, 0, "1302A", 0, 0) == 0)||
	 (CompareStrings (modelnum, 0, "1326", 0, 0) == 0)||
	 (CompareStrings (modelnum, 0, "2322", 0, 0) == 0)||
     (CompareStrings (modelnum, 0, "10710", 0, 0) == 0)||
 	 (CompareStrings (modelnum, 0, "1603", 0, 0) == 0)  )
   Fmt (cmd, "CALCHNL ");  /* Space after command */
 else
   Fmt (cmd, "CALCHNL"); /* No space after command */

 /* Add the enable / disable command to the command string */
 if ((!mode)||(!unit))
   /* Disable calibration mode or enable for single channel unit */
   Fmt (cmd, "%s%d", cmd, mode);
 else
   /* Enabling calibration mode for a dual channel unit */
   if (channel == PPSMPS_CHAN1)
     Fmt (cmd, "%s1", cmd);
   else if (channel == PPSMPS_CHAN2)
     Fmt (cmd, "%s2", cmd);
   else
     Fmt (cmd, "%s3", cmd);

 if (ppsmps_write_data (instrID, cmd, NumFmtdBytes ()) !=0)    /* send command to PS */
     return ppsmps_err;

 return 0; /* return OK */
}

/*=========================================================================*/
/* Function: Write Data with Status Check                                  */
/* Purpose:  This function writes a buffer of data to the instrument. The  */
/*           return value is equal to the status query 'ERROR?'of the Amrel*/
/*           P/S. The return errors can be one of the following.		   */
/*			 	 0 ERROR0 = no error									   */
/*				-1 ERROR1 = command string error 		   				   */
/*			 	-2 ERROR2 = Numeric string out of range					   */
/*				-3 ERROR3 = Numeric string over length					   */
/*				-4 ERROR4 = Command sequence error						   */
/*              -5  ERROR5 & -6 ERROR6 N/A                                 */
/*=========================================================================*/
int ppsmps_write_with_status(int instrID, char *cmd, long cnt)
{
 char buf[50];
 int code;
 char estring[128];   /* string are always passed by reference */


    if (ibwrt(bd[instrID], cmd, NumFmtdBytes ()) <= 0)
        ppsmps_err = 230;   /* error occurred */

  ppsmps_err = ppsmps_query_error_state (instrID, &code, estring);

    if (ppsmps_err >= 0x8000)
      {
       return ppsmps_err;   /* error occurred */
      }

    if (code < 0)
      {
       return code;   /* power supply reported error */
      }
    else
        ppsmps_err = 0;

    return ppsmps_err;

 }

 /*=========================================================================*/
/* Function: close                                                          */
/* Purpose:  This function will close the instrument and release the        */
/*           instrument handle.                 						   */
/*=========================================================================*/
int ppsmps_close( int instrID)
{
 if (ppsmps_close_instr (instrID) !=0)
 return ppsmps_err;


return 0;
}

/*= UTILITY ROUTINES ======================================================*/

/*=========================================================================*/
/* Function: Open Instrument                                               */
/* Purpose:  This function locates and initializes an entry in the         */
/*           Instrument Table and the GPIB device table for the            */
/*           instrument.  If successful, the instrument ID is returned,    */
/*           else a -1 is returned.  The size of the Instrument Table can  */
/*           be changed in the include file by altering the constant       */
/*           PPSMPS_MAX_INSTR.                                             */
/*=========================================================================*/
int ppsmps_open_instr (int addr)
{
    int i, instrID;

    instrID = 0;

/* Check to see if the instrument is already in the Instrument Table. */

    for (i=1; i<= PPSMPS_MAX_INSTR; i++)
        if (address[i] == addr) {
            instrID = i;
            i = PPSMPS_MAX_INSTR;
         }

/* If it is not in the instrument table, open an entry for the instrument. */

    if (instrID <= 0)
        for (i=1; i<= PPSMPS_MAX_INSTR; i++)
            if (address[i] == 0) {
                instrID = i;
                i = PPSMPS_MAX_INSTR;
             }

/* If an entry could not be opened in the Instrument Table, return an error.*/

    if (instrID <= 0) {
        ppsmps_err = 220;
        return -1;
     }

/*  If the device has not been opened in the GPIB device table (bd[ID] = 0),*/
/*  then open it.                                                           */

    if (bd[instrID] <= 0)    {
        if (instr_cnt <= 0)
            CloseInstrDevs("ppsmps");
        bd[instrID] = OpenDev ("", "ppsmps");
        if (bd[instrID] <= 0)    {
            ppsmps_err = 220;
            return -1;
        }
        instr_cnt += 1;
        address[instrID] = addr;
     }

/*  Change the primary address of the device    */

    if (ibpad (bd[instrID], addr) < 0)   {
        ppsmps_err = 233;
        return -1;
    }

    return instrID;
}
/*=========================================================================*/
/* Function: Close Instrument                                              */
/* Purpose:  This function closes the instrument by removing it from the   */
/*           GPIB device table and setting the address and the bd to zero  */
/*           in the Instrument Table.  The return value is equal to the    */
/*           global error variable.                                        */
/*=========================================================================*/
int ppsmps_close_instr (int instrID)
{
    ppsmps_err = 0;

    if (bd[instrID] != 0) {
        CloseDev (bd[instrID]);
        bd[instrID] = 0;
        address[instrID] = 0;
        instr_cnt -= 1;
     }
    else
        ppsmps_err = 221;

    return ppsmps_err;
 }
/*=========================================================================*/
/* Function: Invalid Short Range                                           */
/* Purpose:  This function checks a short to see if it lies between a      */
/*           minimum and maximum value.  If the value is out of range, set */
/*           the global error variable to the value err_code.  If the      */
/*           value is OK, error = 0.                                       */
/*=========================================================================*/
int ppsmps_invalid_short_range (short val, short min, short max, int err_code)
{
  if ((val < min) || (val > max)) {
    ppsmps_err = err_code;
    return -1;
  }
  return 0;
}
/*=========================================================================*/
/* Function: Invalid Integer Range                                         */
/* Purpose:  This function checks an integer to see if it lies between a   */
/*           minimum and maximum value.  If the value is out of range, set */
/*           the global error variable to the value err_code.  If the      */
/*           value is OK, error = 0.                                       */
/*=========================================================================*/
int ppsmps_invalid_integer_range (int val, int min, int max, int err_code)
{
  if ((val < min) || (val > max)) {
    ppsmps_err = err_code;
    return -1;
  }
  return 0;
}
/*=========================================================================*/
/* Function: Invalid Long Integer Range                                    */
/* Purpose:  This function checks a long integer to see if it lies between */
/*           a minimum and maximum value.  If the value is out of range,   */
/*           set the global error variable to the value err_code.  If the  */
/*           value is OK, error = 0.  The return value is equal to the     */
/*           global error value.                                           */
/*=========================================================================*/
int ppsmps_invalid_longint_range (long val, long min, long max, int err_code)
{
    if (val < min || val > max) {
        ppsmps_err = err_code;
        return -1;
    }
    return 0;
}
/*=========================================================================*/
/* Function: Invalid Real Range                                            */
/* Purpose:  This function checks a real number to see if it lies between  */
/*           a minimum and maximum value.  If the value is out of range,   */
/*           set the global error variable to the value err_code.  If the  */
/*           value is OK, error = 0.                                       */
/*=========================================================================*/
int ppsmps_invalid_real_range (double val, double min, double max, int err_code)
{
  if ((val < min) || (val > max)) {
    ppsmps_err = err_code;
    return -1;
  }
  return 0;
}
/*=========================================================================*/
/* Function: Device Closed                                                 */
/* Purpose:  This function checks to see if the module has been            */
/*           initialized.  If the device has not been opened, a 1 is       */
/*           returned, 0 otherwise.                                        */
/*=========================================================================*/
int ppsmps_device_closed (int instrID)
{
    if (bd[instrID] <= 0)  {
        ppsmps_err = 232;
        return -1;
    }
    return 0;
}
/*=========================================================================*/
/* Function: Read Data                                                     */
/* Purpose:  This function reads a buffer of data from the instrument. The */
/*           return value is equal to the global error variable.           */
/*=========================================================================*/
int ppsmps_read_data (int instrID, char *buf, long cnt)
{
    int cr_position;

    if (ibrd(bd[instrID], buf, cnt) <= 0)
        ppsmps_err = 231;
    else
        ppsmps_err = 0;

   /* Convert CR returned by the instrument to an ASCII Null character */
   cr_position = FindPattern (buf, 0, -1, "\n", 0, 0);
   if (cr_position != -1) buf[cr_position-1] = 0;

    return ppsmps_err;
}
/*=========================================================================*/
/* Function: Write Data                                                    */
/* Purpose:  This function writes a buffer of data to the instrument. The  */
/*           return value is equal to the global error variable.           */
/*=========================================================================*/
int ppsmps_write_data (int instrID, char *buf, long cnt)
{
    if (ibwrt(bd[instrID], buf, cnt) <= 0)
        ppsmps_err = 230;
    else
        ppsmps_err = 0;

    return ppsmps_err;
}
/*=========================================================================*/
/* Function: Read Data File                                                */
/* Purpose:  This function reads a buffer of data from the instrument and  */
/*           stores it to the file specified by "filename".  Filename must */
/*           either be a string, such as "C:\lw\instr\file" or a pointer   */
/*           to such a string.  The return value is equal to the global    */
/*           error variable.                                               */
/*=========================================================================*/
int ppsmps_read_data_file (int instrID, char *filename)
{
    if (ibrdf (bd[instrID], filename) <= 0)
        ppsmps_err = 229;
    else
        ppsmps_err = 0;

    return ppsmps_err;
}
/*=========================================================================*/
/* Function: Write Data File                                               */
/* Purpose:  This function writes a buffer of data from the file specified */
/*           by "filename" to the instrument. Filename must either be a    */
/*           string, such as "C:\lw\instr\file" or a pointer to such a     */
/*           string.  The return value is equal to the global error        */
/*           variable.                                                     */
/*=========================================================================*/
int ppsmps_write_data_file (int instrID, char *filename)
{
    if (ibwrtf (bd[instrID], filename) <= 0)
        ppsmps_err = 228;
    else
        ppsmps_err = 0;

    return ppsmps_err;
}
/*=========================================================================*/
/* Function: Serial Poll                                                   */
/* Purpose:  This function performs a serial poll on the instrument.       */
/*           The status byte of the instrument is placed in the response   */
/*           variable. The return value is equal to the global error       */
/*           variable.                                                     */
/*=========================================================================*/
int ppsmps_poll (int instrID, char *response)
{
    if (ibrsp (bd[instrID], response) <= 0)
        ppsmps_err = 226;
    else
        ppsmps_err = 0;

    return ppsmps_err;
}
/*=========================================================================*/
/* Function: Set Timeout                                                   */
/* Purpose:  This function changes or disables the timeout of the device.  */
/*           Refer to the LabWindows Standard Libraries Reference Manual   */
/*           for timeout codes.                                            */
/*=========================================================================*/
int ppsmps_set_timeout (int instrID, int tmo_code)
{
    if (ibtmo (bd[instrID], tmo_code) <= 0)
        ppsmps_err = 239;
    else
        ppsmps_err = 0;

    return ppsmps_err;
}
/*=========================================================================*/
/* Function: Setup Arrays                                                  */
/* Purpose:  This function is called by the init routine to initialize     */
/*           global arrays.                                                */
/*           This routine should be modified for each instrument to        */
/*           include instrument-dependent commmand arrays.                 */
/*=========================================================================*/
void ppsmps_setup_arrays ()
{
  /* This drivers function argument range specific error codes        */
  /* NOTE: Remember to set the PPSMPS_MAX_RANGE_ERROR_NUM constant in */
  /*       if you add new error codes here.                           */
  ppsmps_range_error[0] = "Argument 1 out of range!";  			/* -5001 */
  ppsmps_range_error[1] = "Argument 2 out of range!";  			/* -5002 */
  ppsmps_range_error[2] = "Argument 3 out of range!";  			/* -5003 */
  ppsmps_range_error[3] = "Argument 4 out of range!";  			/* -5004 */
  ppsmps_range_error[4] = "Argument 5 out of range!"; 			/* -5005 */
  ppsmps_range_error[5] = "Argument 6 out of range!";  			/* -5006 */
  ppsmps_range_error[6] = "Argument 7 out of range!";  			/* -5007 */
  ppsmps_range_error[7] = "Argument 8 out of range!";  			/* -5008 */
  ppsmps_range_error[8] = "Argument 9 out of range!";  			/* -5009 */
  ppsmps_range_error[9] = "Argument 10 out of range!"; 			/* -5010 */
  ppsmps_range_error[10] = "Argument 11 out of range!"; 		/* -5011 */
  ppsmps_range_error[11] = "Argument 12 out of range!"; 		/* -5012 */
  ppsmps_range_error[12] = "Argument 13 out of range!"; 		/* -5013 */
  ppsmps_range_error[13] = "Argument 14 out of range!"; 		/* -5014 */
  ppsmps_range_error[14] = "Argument 15 out of range!"; 		/* -5015 */
  ppsmps_range_error[15] = "Argument 16 out of range!"; 		/* -5016 */
}
/*= END =================================================================*/
