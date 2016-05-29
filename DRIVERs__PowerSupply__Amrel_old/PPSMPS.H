/*== <Instrument Model Name> Include File =================================*/

/*== GLOBAL CONSTANT DECLARATIONS =========================================*/

#define PPSMPS_CMD_DELAY        0.055

#define PPSMPS_MAX_INSTR        50

#define PPSMPS_MAX_RANGE_ERROR_NUM 15

#define PPSMPS_RD_VOLTAGE		0
#define PPSMPS_RD_CURRENT		1

#define PPSMPS_SINGLE			0
#define PPSMPS_DUAL				1

#define PPSMPS_CHAN1			0
#define PPSMPS_CHAN2			1
#define PPSMPS_CHAN_BOTH		2

#define PPSMPS_VOLTAGE_OFFSET		0
#define PPSMPS_FULL_SCALE_VOLTS		1
#define PPSMPS_CURRENT_OFFSET		2
#define PPSMPS_FULL_SCALE_CURRENT	3

#define PPSMPS_ERROR_0			"No Errors"
#define PPSMPS_ERROR_1			"Command String Error"
#define PPSMPS_ERROR_2			"Numeric string out of range"
#define PPSMPS_ERROR_3			"Numeric string over length"
#define PPSMPS_ERROR_4			"Command sequence error"
#define PPSMPS_ERROR_5			"Undefined"
#define PPSMPS_ERROR_6			"Undefined"

/*== GLOBAL FUNCTION DECLARATIONS =========================================*/


/** INSERT INSTRUMENT-DEPENDENT FUNCTION DECLARATIONS HERE **/
int ppsmps_initialize (int, int, int, int*, int);
int ppsmps_set_voltage (int, int, int, double, double);
int ppsmps_set_ovp (int, int, int, double, double);
int ppsmps_set_current (int, int, int, double, double);
int ppsmps_set_ovp_enable( int, int, int, int);
int ppsmps_set_ocp_enable( int, int, int, int);
int ppsmps_set_ch_on_off(int, int, int, int);
int ppsmps_set_address( int, int);
int ppsmps_set_port( int, int);
int ppsmps_set_track( int, int);
int ppsmps_enable_cal(int, int, int, int);
int ppsmps_write_cal_data(int, int, double);
int ppsmps_measure( int, int, int, int, double *);
int ppsmps_query_set_curr_volt (int, int, int, int, double *);
int ppsmps_query_error_state (int, int *, char *);
int ppsmps_query_model_number(int, char *);
int ppsmps_close(int);
int ppsmps_configure_dual_CH_all(int, double, double, int, int,
								 int, int, double, double, double,
								 double, int, int, int, int);
int ppsmps_config_single_chan_all(int, double, int, int, double,
						          double, int);
void ppsmps_get_arg_range_error_msg (int, char *);
int ppsmps_query_over_voltage_levels (int, int, int, double *);
/*=============================== END =====================================*/
