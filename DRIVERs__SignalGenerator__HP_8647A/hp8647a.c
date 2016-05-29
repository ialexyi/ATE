/***************************************************************************/
/*  Copyright 1994 National Instruments Corporation.  All Rights Reserved. */
/***************************************************************************/
#include <utility.h>
#include <gpib.h>
#include <formatio.h>
#include "hp8647a.h"

#define hp8647a_REVISION   "Rev 1.0, 3/96, CVI 3.1"

/*=  HP 8647A RF SIGNAL GENERATOR (250 kHz - 1000 MHz) ====================*/
/* LabWindows Instrument Driver                                            */
/* Original Release: August 1994                                           */
/* By: DT, LWILDP, National Instruments, Austin, Texas                     */
/*     Ph (800) 433-3488    Fax (512) 794-5678                             */
/* Originally written in C                                                 */
/* Modification History:                                                   */
/* ========================================================================*/
/*          11/09/94:    Added necessary codes to create DLL               */
/*       Modified by:    SA,  LWILDP                                       */
/*                                                                         */
/*         June 1996:    Modified for MDLL compatibility.                  */
/*                       Modified by:  TKN, National Instruments           */
/*                       Phone (800) 433-3488  Fax (512) 794-5678          */    
/*                                                                         */
/*=========================================================================*/

/*= INSTRUMENT TABLE ======================================================*/
/* address array: contains the GPIB addresses of opened instruments.       */
/* bd array: contains the device descriptors returned by OpenDev.          */
/* instr_cnt: contains the number of instruments open of this model type.  */
/*=========================================================================*/
static int address[hp8647a_MAX_INSTR + 1];
static int bd[hp8647a_MAX_INSTR + 1];
static int instr_cnt;

/*= STATIC VARIABLES ======================================================*/
/* cmd is a buffer for GPIB I/O strings.                                   */
/* hp8647a_err: the error variable for the instrument module               */
/* ibcnt: contains the number of bytes transferred by GPIB reads and       */
/*        writes.  See the GPIB library I/O Class for more information     */
/*=========================================================================*/
static char cmd[200];
static int hp8647a_err;

/*= INSTRUMENT-DEPENDENT COMMAND ARRAYS ===================================*/
static char *function[5];
static char *modul[3];
static char *mod_unt[3];
static char *source[3];
static char *intrnl[2];
static char *func[3];
static char *extrn_coup[2];
static double uppr[3];

/*= UTILITY ROUTINES ======================================================*/
int hp8647a_open_instr (int, int *);
int hp8647a_close_instr (int);
int hp8647a_invalid_integer_range (int, int, int, int);
int hp8647a_invalid_real_range (double, double, double, int);
int hp8647a_device_closed (int);
int hp8647a_read_data (int, char *, int);
int hp8647a_write_data (int, char *, int);
void hp8647a_setup_arrays (void);

/*=========================================================================*/
/* Function: Initialize                                                    */
/* Purpose:  This function opens the instrument, queries the instrument    */
/*           for its ID, and initializes the instrument to a known state.  */
/*=========================================================================*/
int PUBLIC hp8647a_init (int addr, int id_query, int rest, int *instrID)
{
    int  ID;

    if (hp8647a_invalid_integer_range (addr, 0, 30, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (id_query, 0, 1, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (rest, 0, 1, -3) != 0)
        return hp8647a_err;

    if (hp8647a_open_instr (addr, &ID) != 0)
        return hp8647a_err;

    if (id_query) {
        if (hp8647a_write_data (ID, "*IDN?", 5) != 0) {
            hp8647a_close_instr (ID);
            return hp8647a_err;
        }
        if (hp8647a_read_data (ID, cmd, 50) != 0) {
            hp8647a_close_instr (ID);
            return hp8647a_err;
        }

        Scan (cmd, "Hewlett-Packard, 8647A");
        if (NumFmtdBytes () != 22) {
            hp8647a_err = 223;
            hp8647a_close_instr (ID);
            return hp8647a_err;
        }

    }

    if (rest) {
        if (hp8647a_write_data (ID, "*RST", 4) != 0) {
            hp8647a_close_instr (ID);
            return hp8647a_err;
        }
    }
    hp8647a_setup_arrays ();
    *instrID = ID;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    Output RF Signal                                           */
/* Purpose:     Configures the instrument for Modulation. It selects the   */
/*              modulation type (AM, FM or PM) and the modulating signal.  */
/*=========================================================================*/
int PUBLIC hp8647a_output_signal (int instrID, double freq, double ampl, int mod,
                           double mod_vl, int mod_sgnl, int int_freq,
                           int ext_coup)
{
    int total;

    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (freq, 0.250, 1000.0, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (ampl, -136.0, 10.0, -3) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (mod, 0, 2, -4) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (mod_vl, 0.0, uppr[mod], -5) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (mod_sgnl, 0, 2, -6) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (int_freq, 0, 1, -7) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (ext_coup, 0, 1, -8) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    if (mod == 0) {
        if (ampl > 4.0) {
            hp8647a_err = 313;
            return hp8647a_err;
        }
        if (freq <= 1.5) {
            hp8647a_err = 314;
            return hp8647a_err;
        }
    }
    Fmt (cmd, "%s<:POW:ATT:AUTO 1;:FREQ:CW %f MHZ;:POW:REF:STAT 0;", freq);
    total = NumFmtdBytes();

    Fmt (cmd, "%s[a]<:POW:AMPL %f DBM;", ampl);
    total += NumFmtdBytes();

    Fmt (cmd, "%s[a]<%s %f %s;%sSOUR %s;",
               modul[mod], mod_vl, mod_unt[mod], func[mod], source[mod_sgnl]);
    total += NumFmtdBytes();
    if (mod_sgnl != 1) {
        Fmt (cmd, "%s[a]<%sINT:FREQ %s;", func[mod], intrnl[int_freq]);
        total += NumFmtdBytes();
    }
    if (mod_sgnl != 0) {
        if ((mod != 1) & (ext_coup == 0)) {
            hp8647a_err = 312;
            return hp8647a_err;
        }
        Fmt (cmd, "%s[a]<%sEXT:COUP %s;", func[mod], extrn_coup[ext_coup]);
        total += NumFmtdBytes();
    }
    if (mod == 0) {
        Fmt (cmd, "%s[a]<:FM:STAT OFF;:PM:STAT OFF;:AM:STAT ON;:OUTP:STAT 1");
        total += NumFmtdBytes();
    }
    if (mod == 1) {
        Fmt (cmd, "%s[a]<:AM:STAT OFF;:PM:STAT OFF;:FM:STAT ON;:OUTP:STAT 1");
        total += NumFmtdBytes();
    }
    if (mod == 2) {
        Fmt (cmd, "%s[a]<:FM:STAT OFF;:AM:STAT OFF;:PM:STAT ON;:OUTP:STAT 1");
        total += NumFmtdBytes();
    }
    if (hp8647a_write_data (instrID, cmd, total) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    RF Signal                                                  */
/* Purpose:     Configures the RF Signal. It sets the frequency and        */
/*              amplitude of the signal.                                   */
/*=========================================================================*/
int PUBLIC hp8647a_rf_signal (int instrID, double freq, int ref, double ref_vl,
                                    double ampl, int attn)
{
    int total;

    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (freq, 0.250, 1000.0, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (ref, 0, 1, -3) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (ref_vl, -136.0, 10.0, -4) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (attn, 0, 1, -6) != 0)
        return hp8647a_err;

    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    Fmt (cmd, "%s<:POW:ATT:AUTO %d[b4];:FREQ:CW %f MHZ;:POW:REF:STAT %d[b4];",
                                                    attn, freq, ref);
    total = NumFmtdBytes();
    if (ref) {
    if (hp8647a_invalid_real_range (ampl, -136.0 - ref_vl,
                                            10.0 - ref_vl, -5) != 0)
        return hp8647a_err;
    }
    else {
        if (hp8647a_invalid_real_range (ampl, -136.0, 10.0, -5) != 0)
            return hp8647a_err;
    }
    Fmt (cmd, "%s[a]<:POW:REF %f DBM;:POW:AMPL %f DBM;", ref_vl, ampl);
    total += NumFmtdBytes();

    if (hp8647a_write_data (instrID, cmd, total) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    Modulation                                                 */
/* Purpose:     Configures the instrument for Modulation. It selects the   */
/*              modulation type (AM, FM or PM) and the modulating signal.  */
/*=========================================================================*/
int PUBLIC hp8647a_modulation (int instrID, int mod, double mod_vl,
                        int mod_sgnl, int int_freq, int ext_coup)
{
    int total;

    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (mod, 0, 2, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (mod_vl, 0.0, uppr[mod], -3) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (mod_sgnl, 0, 2, -4) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (int_freq, 0, 1, -5) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (ext_coup, 0, 1, -6) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    Fmt (cmd, "%s<%s %f %s;%sSOUR %s;",
               modul[mod], mod_vl, mod_unt[mod], func[mod], source[mod_sgnl]);


    total = NumFmtdBytes();
    if (mod_sgnl != 1) {
        Fmt (cmd, "%s[a]<%sINT:FREQ %s;", func[mod], intrnl[int_freq]);
        total += NumFmtdBytes();
    }
    if (mod_sgnl != 0) {
        if ((mod != 1) & (ext_coup == 0)) {
            hp8647a_err = 312;
            return hp8647a_err;
        }
        Fmt (cmd, "%s[a]<%sEXT:COUP %s;", func[mod], extrn_coup[ext_coup]);
        total += NumFmtdBytes();
    }
    if (mod == 0) {
        Fmt (cmd, "%s[a]<:FM:STAT OFF;:PM:STAT OFF;:AM:STAT ON;");
        total += NumFmtdBytes();
    }
    if (mod == 1) {
        Fmt (cmd, "%s[a]<:AM:STAT OFF;:PM:STAT OFF;:FM:STAT ON;");
        total += NumFmtdBytes();
    }
    if (mod == 2) {
        Fmt (cmd, "%s[a]<:FM:STAT OFF;:AM:STAT OFF;:PM:STAT ON;");
        total += NumFmtdBytes();
    }
    if (hp8647a_write_data (instrID, cmd, total) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    Generate Signal                                            */
/* Purpose:     Sets the instrument to generate the configured RF signal   */
/*              with or without the configured modulation.                 */
/*=========================================================================*/
int PUBLIC hp8647a_gen_signl (int instrID, int mod_all_off, int signl_out)
{
    int total;

    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (mod_all_off, 0, 1, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (signl_out, 0, 1, -3) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    total = 0;
    if (mod_all_off) {
            Fmt (cmd, "%s<:AM:STAT OFF;:PM:STAT OFF;:AM:STAT OFF;");
            total += NumFmtdBytes();
            Fmt (cmd, "%s[a]<:OUTP:STAT %d[b4]", signl_out);
            total += NumFmtdBytes();
    }
    else {
        Fmt (cmd, "%s<:OUTP:STAT %d[b4]", signl_out);
        total += NumFmtdBytes();
    }
    if (hp8647a_write_data (instrID, cmd, total) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Write To Instrument                                           */
/* Purpose:  This function writes a command string to the instrument.      */
/*=========================================================================*/
int PUBLIC hp8647a_write (int instrID, char cmd_string[])
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    Fmt (cmd, "%s<%s", cmd_string);
    if (hp8647a_write_data (instrID, cmd, NumFmtdBytes()) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Read Instrument Buffer                                        */
/* Purpose:  This function reads the output buffer of the instrument.      */
/*=========================================================================*/
int PUBLIC hp8647a_read (int instrID, int numbytes, char in_buff[], int *bytes_read)
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    *bytes_read = 0;
    if (hp8647a_read_data (instrID, in_buff, numbytes) != 0)
        return hp8647a_err;

    *bytes_read = ibcnt;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Reset                                                         */
/* Purpose:  This function resets the instrument.                          */
/*=========================================================================*/
int PUBLIC hp8647a_reset (int instrID)
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    if (hp8647a_write_data (instrID, "*RST", 4) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Self-Test                                                     */
/* Purpose:  This function executes the instrument self-test and returns   */
/* the result.                                                             */
/*=========================================================================*/
int PUBLIC hp8647a_self_test (int instrID, int *test_result, char test_message[])
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    if (hp8647a_write_data (instrID, "*TST?", 5) != 0)
        return hp8647a_err;
    if (hp8647a_read_data (instrID, test_message, 50) != 0)
        return hp8647a_err;

    if (Scan (test_message, "%s>%d[b4]", test_result) != 1) {
        hp8647a_err = 236;
        return hp8647a_err;
    }

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Revision                                                      */
/* Purpose:  This function returns the driver and instrument revisions.    */
/*=========================================================================*/
int PUBLIC hp8647a_revision_query (int instrID, char driver_rev[], char instr_rev[])
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    if (hp8647a_write_data (instrID, "*IDN?", 5) != 0)
        return hp8647a_err;
    if (hp8647a_read_data (instrID, cmd, 50) != 0)
        return hp8647a_err;


    if (Scan (cmd, "%s>Hewlett-Packard, 8647A, %s[d] %s[t10q]", instr_rev) != 2) {
        hp8647a_err = 236;
        return hp8647a_err;
    }
    Fmt (driver_rev, "%s<%s", hp8647a_REVISION);

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    Set Attenuator & Reference                                 */
/* Purpose:     Disables/Enables the Amplitude Reference and the Automatic */
/*              Attenuation modes.                                         */
/*=========================================================================*/
int PUBLIC hp8647a_set_ref_att (int instrID, int ref, int att)
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (att, 0, 1, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (ref, 0, 1, -3) != 0)
        return hp8647a_err;

    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    Fmt (cmd, "%s<:POW:ATT:AUTO %d[b4];:POW:REF:STAT %d[b4]", att, ref);
    if (hp8647a_write_data (instrID, cmd, NumFmtdBytes()) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    Determine Modulation Level                                 */
/* Purpose:     Determines the Modulation level to be set for Depth(AM),   */
/*              Deviation(FM), or Deviation(PM) when the internal and      */
/*              external modulating sources are used with the selected     */
/*              modulation.                                                */
/*=========================================================================*/
int PUBLIC hp8647a_det_mod_set (int mod, double des_mod, double ext_levl, double * mod_set)
{
    if (hp8647a_invalid_integer_range (mod, 0, 2, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (des_mod, 0.0, uppr[mod], -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_real_range (ext_levl, 0.0, 0.5, -3) != 0)
        return hp8647a_err;

    if (ext_levl < 0.5)
        *mod_set = des_mod/(1 + ext_levl);
    else
        *mod_set = (2*des_mod)/3;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function:    Modulation On/Off                                          */
/* Purpose:     Turns the selected modulation On or Off.                   */
/*=========================================================================*/
int PUBLIC hp8647a_mod_on_off (int instrID, int set_mod, int mod)
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (set_mod, 0, 2, -2) != 0)
        return hp8647a_err;
    if (hp8647a_invalid_integer_range (mod, 0, 1, -3) != 0)
        return hp8647a_err;

    if (hp8647a_device_closed(instrID) != 0)
        return hp8647a_err;

    if (mod) {
        if (set_mod == 0)
            Fmt (cmd, "%s<:FM:STAT OFF;:PM:STAT OFF;:AM:STAT ON;");
        if (set_mod == 1)
            Fmt (cmd, "%s<:AM:STAT OFF;:PM:STAT OFF;:FM:STAT ON;");
        if (set_mod == 2)
            Fmt (cmd, "%s<:FM:STAT OFF;:AM:STAT OFF;:PM:STAT ON;");
    }
    else
        Fmt (cmd, "%s<%sSTAT 0", func[set_mod]);

    if (hp8647a_write_data (instrID, cmd, NumFmtdBytes()) != 0)
        return hp8647a_err;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Close                                                         */
/* Purpose:  This function closes the instrument.                          */
/*=========================================================================*/
int PUBLIC hp8647a_close (int instrID)
{
    if (hp8647a_invalid_integer_range (instrID, 1, hp8647a_MAX_INSTR, -1) != 0)
        return hp8647a_err;
    if (hp8647a_device_closed (instrID))
        return hp8647a_err;

    hp8647a_close_instr (instrID);

    return hp8647a_err;
}

/*= UTILITY ROUTINES ======================================================*/

/*=========================================================================*/
/* Function: Open Instrument                                               */
/* Purpose:  This function locates and initializes an entry in the         */
/*           Instrument Table and the GPIB device table for the            */
/*           instrument.  The size of the Instrument Table can be changed  */
/*           in the include file by altering the constant                  */
/*           hp8647a_MAX_INSTR.  The return value of this function is equal */
/*           to the global error variable.                                 */
/*=========================================================================*/
int hp8647a_open_instr (int addr, int *ID)
{
    int i, instrID;

    instrID = 0;
    hp8647a_err = 0;

/* Check to see if the instrument is already in the Instrument Table. */

    for (i = 1; i <= hp8647a_MAX_INSTR; i++)
        if (address[i] == addr) {
            instrID = i;
            i = hp8647a_MAX_INSTR;
         }

/* If it is not in the instrument table, open an entry for the instrument. */

    if (instrID <= 0)
        for (i = 1; i <= hp8647a_MAX_INSTR; i++)
            if (address[i] == 0) {
                instrID = i;
                i = hp8647a_MAX_INSTR;
             }

/* If an entry could not be opened in the Instrument Table, return an error.*/

    if (instrID <= 0) {
        hp8647a_err = 220;
        return hp8647a_err;
    }

/* If the device has not been opened in the GPIB device table (bd[ID] = 0),*/
/*  then open it.                                                          */

    if (bd[instrID] <= 0) {
        if (instr_cnt <= 0)
            CloseInstrDevs("hp8647a");
        bd[instrID] = OpenDev ("", "hp8647a");
        if (bd[instrID] <= 0) {
            hp8647a_err = 220;
            return hp8647a_err;
        }
        instr_cnt += 1;
        address[instrID] = addr;
     }

/*  Change the primary address of the device    */

    if (ibpad (bd[instrID], addr) & 0x8000) {
        hp8647a_err = 233;
        return hp8647a_err;
    }

    *ID = instrID;
    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Close Instrument                                              */
/* Purpose:  This function closes the instrument by removing it from the   */
/*           GPIB device table and setting the address and bd[instrID] to  */
/*           zero in the Instrument Table.  The return value is equal to   */
/*           the global error variable.                                    */
/*=========================================================================*/
int hp8647a_close_instr (int instrID)
{
    if (bd[instrID] != 0) {
        CloseDev (bd[instrID]);
        bd[instrID] = 0;
        address[instrID] = 0;
        instr_cnt -= 1;
    }
    else
        hp8647a_err = 221;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Invalid Integer Range                                         */
/* Purpose:  This function checks an integer to see if it lies between a   */
/*           minimum and maximum value.  If the value is out of range, set */
/*           the global error variable to the value err_code.  If the      */
/*           value is OK, error = 0.  The return value is equal to the     */
/*           global error value.                                           */
/*=========================================================================*/
int hp8647a_invalid_integer_range (int val, int min, int max, int err_code)
{
    if (val < min || val > max)
        hp8647a_err = err_code;
    else
        hp8647a_err = 0;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Invalid Real Range                                            */
/* Purpose:  This function checks a real number to see if it lies between  */
/*           a minimum and maximum value.  If the value is out of range,   */
/*           set the global error variable to the value err_code.  If the  */
/*           value is OK, error = 0.  The return value is equal to the     */
/*           global error value.                                           */
/*=========================================================================*/
int hp8647a_invalid_real_range (double val, double min, double max, int err_code)
{
    if (val < min || val > max)
        hp8647a_err = err_code;
    else
        hp8647a_err = 0;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Device Closed                                                 */
/* Purpose:  This function checks to see if the module has been            */
/*           initialized.  If the device has not been opened, set the      */
/*           global error variable to 232, 0 otherwise. The return value   */
/*           is equal to the global error value.                           */
/*=========================================================================*/
int hp8647a_device_closed (int instrID)
{
    if (bd[instrID] <= 0)
        hp8647a_err = 232;
    else
        hp8647a_err = 0;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Read Data                                                     */
/* Purpose:  This function reads a buffer of data from the instrument. The */
/*           return value is equal to the global error variable.           */
/*=========================================================================*/
int hp8647a_read_data (int instrID, char *buf, int cnt)
{
    if (ibrd(bd[instrID], buf, (long)cnt) & 0x8000)
        hp8647a_err = 231;
    else
        hp8647a_err = 0;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Write Data                                                    */
/* Purpose:  This function writes a buffer of data to the instrument. The  */
/*           return value is equal to the global error variable.           */
/*=========================================================================*/
int hp8647a_write_data (int instrID, char *buf, int cnt)
{
    if (ibwrt(bd[instrID], buf, (long)cnt) & 0x8000)
        hp8647a_err = 230;
    else
        hp8647a_err = 0;

    return hp8647a_err;
}

/*=========================================================================*/
/* Function: Setup Arrays                                                  */
/* Purpose:  This function is called by the init routine to initialize     */
/*           static arrays.                                                */
/*=========================================================================*/
void hp8647a_setup_arrays ()
{
    modul[0] = ":AM:DEPT";
    modul[1] = ":FM:DEV";
    modul[2] = ":PM:DEV";

    mod_unt[0] = "PCT";
    mod_unt[1] = "KHZ";
    mod_unt[2] = "RAD";

    source[0] = "INT";
    source[1] = "EXT";
    source[2] = "INT,EXT";

    intrnl[0] = "400 HZ";
    intrnl[1] = "1 KHZ";

    func[0] = ":AM:";
    func[1] = ":FM:";
    func[2] = ":PM:";

    extrn_coup[0] = "DC";
    extrn_coup[1] = "AC";

    uppr[0] = 100.0;
    uppr[1] = 100.0;
    uppr[2] = 10.0;

}

/*=== THE END =============================================================*/
