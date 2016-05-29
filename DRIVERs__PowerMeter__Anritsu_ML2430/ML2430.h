#ifndef __ml2430_h__
#define __ml2430_h__


#include <ansi_c.h>
#include <vpptype.h>
#include <ivi.h>
#include <formatio.h>
#include <userint.h>


#define BUFFER_SIZE 512

#define ML2430_OPC 0x01
#define ML2430_RQC 0x02
#define ML2430_QYE 0x04
#define ML2430_DDE 0x08
#define ML2430_EXE 0x10
#define ML2430_CME 0x20
#define ML2430_URQ 0x40
#define ML2430_PON 0x80

#define ML2430_MAV 0x10
#define ML2430_ESB 0x20
#define ML2430_RQS 0x40


struct SRQConditions
{
	ViBoolean OpComp;	   // OPC
	ViBoolean ReqCont;	   // RQC
	ViBoolean QuerErr;	   // QYE
	ViBoolean DevDepErr;   // DDE
	ViBoolean ExeErr;	   // EXE
	ViBoolean ComErr;	   // CME
	ViBoolean UsrReq;	   // URQ
	ViBoolean PowOn;	   // PON
	ViBoolean MsgAvail;		 // MAV
};

struct ErrorListSensor
{
	ViInt32 ZeroState;
	ViInt32 CALState;
	ViInt32 RangeHold;
	ViInt32 FittedUsedState;
};

struct ErrorListChannel
{
	ViInt32 OutOfRange;
	ViInt32 IllegalLogOp;
	ViInt32 LimitsState;
};

struct ChannelStatus
{
	ViUInt8 InptCnfg;
	ViUInt8 Unts;
	ViUInt8 RelSt;
	ViUInt8 LLSt;
	ViUInt8 HLSt;
};

struct SensorStatus
{
	ViUInt16 MeasMde;
	ViUInt16 RngeHld;
	ViUInt16 AvgMde;
	ViUInt16 AvgNum;
	ViUInt16 LLAvg;
	ViUInt16 ZerSt;
};

struct GPIBStatus
{
	ViUInt8 TrgMde;
	ViUInt8 GrpTrgMde;
	ViUInt8 CalSt;
	ViUInt8 DspComSt;
	ViUInt8 FastSt;
};


enum Polarity {NegPolarity, PosPolarity};
enum AutoDial {AD_Disabled, AD_Enabled};
enum SensorSelect {SensorA, SensorB};
enum AveragingMode {Mode_Off, Mode_Low, Mode_Medium, Mode_High};
enum AveragingType {Type_Moving, Type_Auto};
enum AveragingTypeMode {TypeMode_Off, TypeMode_Mov, TypeMode_Rpt, 
						TypeMode_Auto};;
enum BatteryAuto {BA_Disable, BA_Enable};
enum BatteryLCDMode {LCD_On, LCD_Off, LCD_Timed};
enum ChannelSelect {Channel_1, Channel_2, Channel_1_2};
enum BNCUnits {BNC_W, BNC_DB, BNC_DBM, BNC_DBUV, BNC_DBMV};
enum BNCOutputMode {BNCMode_OFF, BNCMode_AOUT, BNCMode_PASS_FAIL, BNCMode_SIGA,
					BNCMode_LVLA1, BNCMode_LVLA2, BNCMode_LVLB1, BNCMode_LVLB2,
					BNCMode_ACMOD, BNCMode_RFB, BNCMode_SIGB};
enum PortSelect {Port_1, Port_2};
enum HighLowLevel {PFL_High, PFL_Low};
enum CalUnits {Cal_Percent, Cal_PCT, Cal_DB, Cal_DBM};
enum CalFrequency {Frq_Hz, Frq_kHz, Frq_MHz, Frq_GHz};
enum CalSource {Src_FREQ, Src_MAN, Src_VGHZ};
enum CalValidity {CalInvalid, CalValid};
enum ChannelInputConfig {Config_OFF, Config_A, Config_B, Config_V, Config_A_B, 
						 Config_B_A, Config_AslashB, Config_BslashA};
enum ChannelResolution {Res_1, Res_2, Res_3};
enum ChannelUnits {Chan_Watts, Chan_dB, Chan_dBuV, Chan_dBmV};
enum CursorSelect {Cursor_1, Cursor_2};
enum TimeUnits {Time_Minutes, Time_Hours};
enum ContrastStep {Step_Up, Step_Down};
enum EmulationSetting {Emul_ML24XX, Emul_HP436A, Emul_HP437B, Emul_HP438A,
					   Emul_ML4803};
enum GETTriggerType {Type_Ignore, Type_TR1, Type_TR2};
enum GETTriggerMode {Mode_Hold, Mode_Immediate, Mode_WithSettlingDelay, 
					 Mode_FreeRun};
enum OperationMode {Mode_Digit, Mode_Profile, Mode_Pwrtim, Mode_Srcswp};
enum PeakDisplay {PkDsp_Off, PkDsp_A, PkDsp_B, PkDsp_AB};
enum PrinterSelect {Printer_HP340, Printer_BJC80};
enum ProfileTrigger {Profile_Rise, Profile_Fall};
enum ProfileTracking {Profile_Single, Profile_Infinite};
enum ProfileResetMode {Profile_MinMaxValues, Profile_SweepToSweep};
enum ProfileSource {Profile_INTA, Profile_INTB, Profile_EXTTTL, Profile_MANUAL,
					Profile_CONT};
enum ProfileType {Profile_NORM, Profile_MINMAX, Profile_MIN, Profile_MAX, 
				  Profile_AVG};
enum RelativeControl {RelControl_OFF, RelControl_ON_NewRef,	
					  RelControl_ON_OldRef};
enum RS232Rate {Baud_1200, Baud_2400, Baud_4800, Baud_9600, Baud_19200, 
				Baud_38400};
enum OperatingMode {Mode_ExtComm, Mode_SrcSwp};
enum OffsetType {Type_Off, Type_Fixed, Type_Table};
enum SensorMeasMode {Mode_DEFAULT, Mode_MOD, Mode_CUSTOM};
enum ProfileOrReadout {PR_Profile, PR_Readout};
enum SourceSweepMode {Mode_FREQ, Mode_POWER};
enum StartupSelfTest {Test_FAILED, Test_PASSED_AND_RUNNING, Test_RUNNING};
enum TriggerMode {Mode_Individual, Mode_Combined};
enum ZeroedState {Not_Zeroed, Zeroed_Zeroed};
enum GroupTriggerMode {Mode_GT0, Mode_GT1, Mode_GT2};
//Error Listing enums
enum ZeroStateEnum {ZeroDone, NotDone, ZeroFailed};
enum CalStateEnum {CalDone, CalFailed};
enum RangeHoldEnum {RangeOK, OverRange, UnderRange};
enum FittedUsedEnum {FittedFitted, NotFittedUsed};
enum LogOperation {LogOK, LogError};
enum LimitsState {LimitsPassed, LimitsHiFailed, LimitsLoFailed};

enum SwitchOnOff {Switch_Off, Switch_On};


ViStatus _VI_FUNC ML2430_Initialize( ViChar *Resource , ViInt32 Rst, ViSession *vi);

ViStatus _VI_FUNC ML2430_Close(ViSession *vi);
ViStatus _VI_FUNC ML2430_SetACModOutputConfig(ViSession vi, ViInt32 Plrty);
ViStatus _VI_FUNC ML2430_GetACModOutputConfig(ViSession vi, ViInt32 *Plrty);
ViStatus _VI_FUNC ML2430_AutoScaling(ViSession vi);
ViStatus _VI_FUNC ML2430_SetAutodialEnableForPower(ViSession vi, ViInt32 AutoD);
ViStatus _VI_FUNC ML2430_GetAutodialEnableForPower(ViSession vi, ViInt32 *AutoD);
ViStatus _VI_FUNC ML2430_SetAutodialEnableForRange(ViSession vi, ViInt32 AutoD);
ViStatus _VI_FUNC ML2430_GetAutodialEnableForRange(ViSession vi, ViInt32 *AutoD);
ViStatus _VI_FUNC ML2430_SetAutodialEnable(ViSession vi, ViInt32 AutoD);
ViStatus _VI_FUNC ML2430_GetAutodialEnable(ViSession vi, ViInt32 *AutoD);
ViStatus _VI_FUNC ML2430_SetAutodialPhoneNumber(ViSession vi, ViChar Strng[]);
ViStatus _VI_FUNC ML2430_GetAutodialPhoneNumber(ViSession vi, ViInt32 MaxLngth, 
										   		ViChar Strng[]);
ViStatus _VI_FUNC ML2430_SetAveragingAutoLowLevel(ViSession vi, ViInt32 Snsr,
												  ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetAveragingAutoLowLevel(ViSession vi, ViInt32 ISnsr,
												  ViInt32 *OSnsr, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_AveragingMoving(ViSession vi, ViInt32 AvgType);
ViStatus _VI_FUNC ML2430_SetAveragingSetup(ViSession vi, ViInt32 Snsr, 
										   ViInt32 AvgType, ViUInt16 NumRds);
ViStatus _VI_FUNC ML2430_GetAveragingSetup(ViSession vi, ViInt32 ISnsr, 
										   ViInt32 *OSnsr, ViInt32 *AvgType, 
										   ViInt32 *NumRds);
ViStatus _VI_FUNC ML2430_AvgBetweenProfileCursors(ViSession vi, ViInt32 Avging);
ViStatus _VI_FUNC ML2430_SetBackLightTimer(ViSession vi, ViReal64 Tme);
ViStatus _VI_FUNC ML2430_GetBackLightTimer(ViSession vi, ViReal64 *Tme);
ViStatus _VI_FUNC ML2430_SetBatteryAutoTurnOffMinutes(ViSession vi, ViUInt8 Min);
ViStatus _VI_FUNC ML2430_GetBatteryAutoTurnOffMinutes(ViSession vi,ViUInt8 *Min);
ViStatus _VI_FUNC ML2430_SetBatteryAutoTurnOff(ViSession vi, ViInt32 AutoOff);
ViStatus _VI_FUNC ML2430_GetBatteryAutoTurnOff(ViSession vi, ViInt32 *AutoOff);
ViStatus _VI_FUNC ML2430_SetBatteryLCDBackLightMode(ViSession vi, ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetBatteryLCDBackLightMode(ViSession vi, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SetBlankingInputPolarity(ViSession vi, ViInt32 Plrty);
ViStatus _VI_FUNC ML2430_GetBlankingInputPolarity(ViSession vi, ViInt32 *Plrty);
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutDisplayStart(ViSession vi, ViInt32 Chan, 
													 ViInt32 Unit, ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetBNCAnalogOutDisplayStart(ViSession vi,
													 ViInt32 IChan,
													 ViInt32 *OChan,
													 ViInt32 *Unit, 
													 ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutDisplayStop(ViSession vi, ViInt32 Chan, 
													ViInt32 Unit, ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetBNCAnalogOutDisplayStop(ViSession vi, ViInt32 IChan,
													ViInt32 *OChan,
													ViInt32 *Unit,ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutputStartVolt(ViSession vi, ViInt32 Port, 
													 ViReal64 Strt);
ViStatus _VI_FUNC ML2430_GetBNCAnalogOutputStartVolt(ViSession vi, 
													 ViInt32 IPort,
													 ViInt32 *OPort, 
													 ViReal64 *Strt);
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutputStopVolt(ViSession vi, ViInt32 Port, 
													ViReal64 Stp);
ViStatus _VI_FUNC ML2430_GetBNCAnalogOutputStopVolt(ViSession vi, ViInt32 IPort,
													ViInt32 *OPort, 
													ViReal64 *Stp);
ViStatus _VI_FUNC ML2430_SetBNCOutPortChannel(ViSession vi, ViInt32 Port, 
											  ViInt32 Chan);
ViStatus _VI_FUNC ML2430_GetBNCOutPortChannel(ViSession vi, ViInt32 Port, 
											  ViInt32 *Chan);
ViStatus _VI_FUNC ML2430_SetBNCOutputMode(ViSession vi, ViInt32 Chan,
										  ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetBNCOutputMode(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SetBNCPassLevel(ViSession vi, ViInt32 Port, 
										 ViInt32 Lvl);
ViStatus _VI_FUNC ML2430_GetBNCPassLevel(ViSession vi, ViInt32 Port, 
										 ViInt32 *Lvl);
ViStatus _VI_FUNC ML2430_SetBNCRFBlanking(ViSession vi, ViInt32 Lvl);
ViStatus _VI_FUNC ML2430_GetBNCRFBlanking(ViSession vi, ViInt32 *Lvl);
ViStatus _VI_FUNC ML2430_CalAddEntry(ViSession vi, ViInt32 Snsr, ViUInt8 Tbl,
									 ViReal64 Frq, ViReal64 Val, ViInt32 Unit);
ViStatus _VI_FUNC ML2430_SetCalAdjust(ViSession vi, ViInt32 Snsr, ViInt32 Unit, 
									  ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalAdjust(ViSession vi, ViInt32 ISnsr, 
									  ViInt32 *OSnsr, ViInt32 *Unit, 
									  ViReal64 *Val);
ViStatus _VI_FUNC ML2430_CalCurrentValue(ViSession vi, ViInt32 Snsr, 
										 ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetCalDisplayUnits(ViSession vi, ViInt32 Snsr, 
											ViInt32 Unit);
ViStatus _VI_FUNC ML2430_GetCalDisplayUnits(ViSession vi, ViInt32 ISnsr, 
											ViInt32 *OSnsr, ViInt32 *Unit);
ViStatus _VI_FUNC ML2430_SetCalFrequency(ViSession vi, ViInt32 Snsr, 
										 ViInt32 Unit, ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalFrequency(ViSession vi, ViInt32 ISnsr, 
										 ViInt32 *OSnsr, ViInt32 *Unit, 
										 ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetCalManual(ViSession vi, ViInt32 Snsr, ViInt32 Unit, 
									  ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalManual(ViSession vi, ViInt32 ISnsr, 
									  ViInt32 *OSnsr, ViInt32 *Unit, 
									  ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetCalSource(ViSession vi, ViInt32 Snsr, ViInt32 Src);
ViStatus _VI_FUNC ML2430_GetCalSource(ViSession vi, ViInt32 ISnsr, 
									  ViInt32 *OSnsr, ViInt32 *Src);
ViStatus _VI_FUNC ML2430_SetCalStartFrequency(ViSession vi, ViInt32 Snsr, 
										 	  ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalStartFrequency(ViSession vi, ViInt32 Snsr, 
										 	  ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetCalStartVoltage(ViSession vi, ViInt32 Snsr, 
										 	ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalStartVoltage(ViSession vi, ViInt32 Snsr, 
										 	ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetCalStopFrequency(ViSession vi, ViInt32 Snsr, 
										 	 ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalStopFrequency(ViSession vi, ViInt32 Snsr, 
										 	 ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetCalStopVoltage(ViSession vi, ViInt32 Snsr, 
										   ViReal64 Val);
ViStatus _VI_FUNC ML2430_GetCalStopVoltage(ViSession vi, ViInt32 Snsr, 
										 	  ViReal64 *Val);
ViStatus _VI_FUNC ML2430_CalTableClear(ViSession vi, ViInt32 Snsr, 
									   ViUInt8 TblNum);
ViStatus _VI_FUNC ML2430_SetCalTableIdentity(ViSession vi, ViInt32 Snsr, 
											 ViUInt8 Tbl, ViChar Ident[]);
ViStatus _VI_FUNC ML2430_GetCalTableIdentity(ViSession vi, ViInt32 ISnsr, 
										 	 ViInt32 *OSnsr, ViUInt8 Tbl, 
										 	 ViChar Ident[]);
ViStatus _VI_FUNC ML2430_SetCalTableLoadRead(ViSession vi, ViInt32 Snsr, 
											 ViUInt8 Tbl, ViChar Ident[],
											 ViReal64 Freq[], ViReal64 Cal[],
											 ViUInt32 Sze);
ViStatus _VI_FUNC ML2430_GetCalTableLoadRead(ViSession vi, ViInt32 Snsr, 
										 	 ViUInt8 Tbl, ViChar Ident[],
										 	 ViReal64 Freq[], ViReal64 Cal[],
										 	 ViInt32 Sze);
ViStatus _VI_FUNC ML2430_CalTablePreset(ViSession vi, ViInt32 Snsr, 
									   ViUInt8 TblNum);
ViStatus _VI_FUNC ML2430_CalTableSave(ViSession vi);
ViStatus _VI_FUNC ML2430_CalTableSelect(ViSession vi, ViInt32 Snsr, 
									    ViUInt8 TblNum);
ViStatus _VI_FUNC ML2430_CalTablesCount(ViSession vi, ViInt32 Snsr, 
									    ViUInt8 *TblNum);
ViStatus _VI_FUNC ML2430_CalTablesUsed(ViSession vi, ViInt32 Snsr, 
									   ViUInt8 *TblUsd);
ViStatus _VI_FUNC ML2430_CalValidityCheck(ViSession vi, ViInt32 Snsr, 
									      ViUInt8 TblNum, ViInt32 *Validty);
ViStatus _VI_FUNC ML2430_CalibrateSensor(ViSession vi, ViInt32 Snsr);
ViStatus _VI_FUNC ML2430_SetChannelInputConfig(ViSession vi, ViInt32 Chan, 
											   ViInt32 Config);
ViStatus _VI_FUNC ML2430_GetChannelInputConfig(ViSession vi, ViInt32 IChan, 
										 	   ViInt32 *OChan, ViInt32 *Config);
ViStatus _VI_FUNC ML2430_SetChannelResolution(ViSession vi, ViInt32 Chan, 
											  ViInt32 Res);
ViStatus _VI_FUNC ML2430_GetChannelResolution(ViSession vi, ViInt32 IChan, 
										 	  ViInt32 *OChan, ViInt32 *Res);
ViStatus _VI_FUNC ML2430_SetChannelUnits(ViSession vi, ViInt32 Chan, 
										 ViInt32 Unit);
ViStatus _VI_FUNC ML2430_GetChannelUnits(ViSession vi, ViInt32 IChan, 
										 ViInt32 *OChan, ViInt32 *Unit);
ViStatus _VI_FUNC ML2430_ConditionsForSRQ(ViSession vi, 
										  struct SRQConditions SRQCnd);
ViStatus _VI_FUNC ML2430_SetConnectPointsOnProfile(ViSession vi, ViInt32 Avgng);
ViStatus _VI_FUNC ML2430_GetConnectPointsOnProfile(ViSession vi, ViInt32 *Avgng);
ViStatus _VI_FUNC ML2430_Continue(ViSession vi);
ViStatus _VI_FUNC ML2430_Cursor(ViSession vi, ViInt32 Crsr, ViReal64 Val);
ViStatus _VI_FUNC ML2430_SetDataDisplayTime(ViSession vi, ViReal64 Tme, 
											ViInt32 Unit);
ViStatus _VI_FUNC ML2430_GetDataDisplayTime(ViSession vi, ViReal64 *Tme);
ViStatus _VI_FUNC ML2430_DisplayContrastStep(ViSession vi, ViInt32 Stp);
ViStatus _VI_FUNC ML2430_SetDisplayContrast(ViSession vi, ViUInt8 Cntrst);
ViStatus _VI_FUNC ML2430_GetDisplayContrast(ViSession vi, ViUInt8 *Cntrst);
ViStatus _VI_FUNC ML2430_SetDisplayGraphCursor(ViSession vi, ViInt32 Crsr);
ViStatus _VI_FUNC ML2430_GetDisplayGraphCursor(ViSession vi, ViInt32 *Crsr);
ViStatus _VI_FUNC ML2430_SetDisplayOnOff(ViSession vi, ViInt32 Dsp);
ViStatus _VI_FUNC ML2430_GetDisplayOnOff(ViSession vi, ViInt32 *Dsp);
ViStatus _VI_FUNC ML2430_SetDisplayTriggerDelay(ViSession vi, ViReal64 DspDly);
ViStatus _VI_FUNC ML2430_GetDisplayTriggerDelay(ViSession vi, ViReal64 *DspDly);
ViStatus _VI_FUNC ML2430_Display(ViSession vi, ViInt32 Dsp, ViInt32 Cntrst,
								 ViInt32 PkDsp);
ViStatus _VI_FUNC ML2430_SetDutyCycleState(ViSession vi, ViInt32 Snsr,
										   ViInt32 St);
ViStatus _VI_FUNC ML2430_GetDutyCycleState(ViSession vi, ViInt32 ISnsr,
										   ViInt32 *OSnsr, ViInt32 *St);
ViStatus _VI_FUNC ML2430_SetDutyCycle(ViSession vi, ViInt32 Snsr, ViReal64 Cyc);
ViStatus _VI_FUNC ML2430_GetDutyCycle(ViSession vi, ViInt32 ISnsr,
									  ViInt32 *OSnsr, ViReal64 *Cyc);
ViStatus _VI_FUNC ML2430_Emulation(ViSession vi, ViInt32 Emul);
ViStatus _VI_FUNC ML2430_SetEntryErrorBeep(ViSession vi, ViInt32 Beep);
ViStatus _VI_FUNC ML2430_GetEntryErrorBeep(ViSession vi, ViInt32 *Beep);
ViStatus _VI_FUNC ML2430_ErrorListing(ViSession vi, 
									  struct ErrorListSensor *SnsrA,
									  struct ErrorListSensor *SnsrB, 
									  struct ErrorListChannel *Chan1,
									  struct ErrorListChannel *Chan2,
									  ViInt32 *PrntrErr, ViChar ExeErr[8],
									  ViChar ComErr[8]);
ViStatus _VI_FUNC ML2430_SetFailBeepOnOff(ViSession vi, ViInt32 Chan, 
										  ViInt32 Beep);
ViStatus _VI_FUNC ML2430_GetFailBeepOnOff(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Beep);
ViStatus _VI_FUNC ML2430_SetFailIndicatorHold(ViSession vi, ViInt32 Chan, 
										  	  ViInt32 Fail);
ViStatus _VI_FUNC ML2430_GetFailIndicatorHold(ViSession vi, ViInt32 IChan, 
										  	  ViInt32 *OChan, ViInt32 *Fail);
ViStatus _VI_FUNC ML2430_Fast(ViSession vi, ViInt32 Fast);
ViStatus _VI_FUNC ML2430_SetFrequencyOffsetDisplay(ViSession vi, ViInt32 Dsp);
ViStatus _VI_FUNC ML2430_GetFrequencyOffsetDisplay(ViSession vi, ViInt32 *Dsp);
ViStatus _VI_FUNC ML2430_GETConfiguration(ViSession vi, ViInt32 Get, 
										  ViInt32 Mode);
ViStatus _VI_FUNC ML2430_SetGPIBAddress(ViSession vi, ViInt32 Addr);
ViStatus _VI_FUNC ML2430_GetGPIBAddress(ViSession vi, ViInt32 *Addr);
ViStatus _VI_FUNC ML2430_GPIBGETTriggerType(ViSession vi, ViInt32 Get);
ViStatus _VI_FUNC ML2430_GPIBResponseBuffering(ViSession vi, ViInt32 RspB);
ViStatus _VI_FUNC ML2430_GPIBTriggerMode(ViSession vi, ViInt32 Mode, 
										 ViInt32 Chan);
ViStatus _VI_FUNC ML2430_GraphDataInformation(ViSession vi, ViUInt32 *ChanNum,
											  ViReal64 *Crsr1, ViReal64 *Crsr2,
											  ViReal64 *DltaPow, 
											  ViReal64 *DltaTme, ViReal64 *Avg, 
											  ViReal64 *MinVal, 
											  ViReal64 *MaxVal);
ViStatus _VI_FUNC ML2430_SetGraphPretriggerPercentage(ViSession vi, ViUInt8 Pct);
ViStatus _VI_FUNC ML2430_GetGraphPretriggerPercentage(ViSession vi, 
													  ViUInt8 *Pct);
ViStatus _VI_FUNC ML2430_SetHold(ViSession vi, ViInt32 Hld);
ViStatus _VI_FUNC ML2430_GetHold(ViSession vi, ViInt32 *Hld);
ViStatus _VI_FUNC ML2430_Identification(ViSession vi, ViChar Ident[32]);
ViStatus _VI_FUNC ML2430_TestSelf(ViSession vi, ViInt32 *Sttus);
ViStatus _VI_FUNC ML2430_SetOperationComplete(ViSession vi);
ViStatus _VI_FUNC ML2430_GetOperationComplete(ViSession vi, ViInt32 *Sttus);
ViStatus _VI_FUNC ML2430_ClearStatusBytes(ViSession vi);
ViStatus _VI_FUNC ML2430_SetEventStatusByteEnable(ViSession vi, ViUInt8 Msk);
ViStatus _VI_FUNC ML2430_GetEventStatusByteEnable(ViSession vi, ViUInt8 *Byt);
ViStatus _VI_FUNC ML2430_EventStatusRegister(ViSession vi, ViUInt8 *Byt);
ViStatus _VI_FUNC ML2430_SetServiceRequestRegister(ViSession vi, ViUInt8 Msk);
ViStatus _VI_FUNC ML2430_GetServiceRequestRegister(ViSession vi, ViUInt8 *Byt);
ViStatus _VI_FUNC ML2430_StatusByteRegister(ViSession vi, ViUInt8 *Byt);
ViStatus _VI_FUNC ML2430_GroupExecuteTrigger(ViSession vi, ViChar Buf[], 
											 ViInt32 Bufsize);
ViStatus _VI_FUNC ML2430_InitializeModem(ViSession vi);
ViStatus _VI_FUNC ML2430_SetKeyClickSoundOnOff(ViSession vi, ViInt32 Snd);
ViStatus _VI_FUNC ML2430_GetKeyClickSoundOnOff(ViSession vi, ViInt32 *Snd);
ViStatus _VI_FUNC ML2430_SetLimitsOnOff(ViSession vi, ViInt32 Chan, 
										ViInt32 HLim, ViInt32 LLim);
ViStatus _VI_FUNC ML2430_GetLimitsOnOff(ViSession vi, ViInt32 Chan, 
										ViInt32 *HLim, ViInt32 *LLim);
ViStatus _VI_FUNC ML2430_SetLimitsSetting(ViSession vi, ViInt32 Chan, 
										  ViReal64 HLim, ViReal64 LLim);
ViStatus _VI_FUNC ML2430_GetLimitsSetting(ViSession vi, ViInt32 Chan, 
										  ViReal64 *HLim, ViReal64 *LLim);
ViStatus _VI_FUNC ML2430_SetLinkGraphCursors(ViSession vi, ViInt32 Lnk);
ViStatus _VI_FUNC ML2430_GetLinkGraphCursors(ViSession vi, ViInt32 *Lnk);
ViStatus _VI_FUNC ML2430_SetMinMaxTracking(ViSession vi, ViInt32 Chan, 
										   ViInt32 Mnmx);
ViStatus _VI_FUNC ML2430_GetMinMaxTracking(ViSession vi, ViInt32 IChan, 
										   ViInt32 *OChan, ViInt32 *Mnmx);
ViStatus _VI_FUNC ML2430_MinMaxTrackingReset(ViSession vi, ViInt32 Chan);
ViStatus _VI_FUNC ML2430_SetModemRedialDelayTime(ViSession vi, ViUInt8 Dly);
ViStatus _VI_FUNC ML2430_GetModemRedialDelayTime(ViSession vi, ViUInt8 *Dly);
ViStatus _VI_FUNC ML2430_SetOffsetFixedValue(ViSession vi, ViInt32 Snsr,
											 ViReal64 OffSt);
ViStatus _VI_FUNC ML2430_GetOffsetFixedValue(ViSession vi, ViInt32 ISnsr,
											 ViInt32 *OSnsr, ViReal64 *OffSt);
ViStatus _VI_FUNC ML2430_OffsetTableClear(ViSession vi, ViUInt8 Tbl);
ViStatus _VI_FUNC ML2430_SetOffsetTableOutputUpdate(ViSession vi, 
													ViUInt8 TblNum,
											 		ViReal64 Freq[],
											 		ViReal64 OffSt[], 
											 		ViInt32 Sze);
ViStatus _VI_FUNC ML2430_GetOffsetTableOutputUpdate(ViSession vi, 
													ViUInt8 TblNum,
											 		ViReal64 Freq[],
											 		ViReal64 OffSt[], 
											 		ViInt32 Sze);
ViStatus _VI_FUNC ML2430_SetOffsetTable(ViSession vi, ViInt32 Snsr, ViUInt8 Tbl);
ViStatus _VI_FUNC ML2430_GetOffsetTable(ViSession vi, ViInt32 ISnsr, 
										ViInt32 *OSnsr, ViUInt8 *Tbl);
ViStatus _VI_FUNC ML2430_SetOperationMode(ViSession vi, ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetOperationMode(ViSession vi, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_OutputGraphBinaryData(ViSession vi, ViReal64 Dta[],
											   ViInt32 Sze);
ViStatus _VI_FUNC ML2430_OutputGraphData(ViSession vi, ViReal64 Dta[200]);
ViStatus _VI_FUNC ML2430_OutputMaxGraphBinaryData(ViSession vi, ViReal64 Dta[],
												  ViInt32 Sze);
ViStatus _VI_FUNC ML2430_OutputMaxGraphData(ViSession vi, ViReal64 Dta[200]);
ViStatus _VI_FUNC ML2430_OutputMinGraphBinaryData(ViSession vi, ViReal64 Dta[],
												  ViInt32 Sze);
ViStatus _VI_FUNC ML2430_OutputMinGraphData(ViSession vi, ViReal64 Dta[200]);
ViStatus _VI_FUNC ML2430_OutputNumberOfChannelReadings(ViSession vi, 
													   ViInt32 Chan,
													   ViUInt16 NumRds,
													   ViReal64 Rslt[]);
ViStatus _VI_FUNC ML2430_OutputOneReading(ViSession vi, ViInt32 Chan, 
										  ViReal64 *Rslt);
ViStatus _VI_FUNC ML2430_OutputValidSampleArray(ViSession vi);
ViStatus _VI_FUNC ML2430_SetPeakMeterDisplay(ViSession vi, ViInt32 PkDsp);
ViStatus _VI_FUNC ML2430_GetPeakMeterDisplay(ViSession vi, ViInt32 *PkDsp);
ViStatus _VI_FUNC ML2430_Print(ViSession vi);
ViStatus _VI_FUNC ML2430_SetPrinterSelect(ViSession vi, ViInt32 Slct);
ViStatus _VI_FUNC ML2430_GetPrinterSelect(ViSession vi, ViInt32 *Slct);
ViStatus _VI_FUNC ML2430_SetProfileChannelSelection(ViSession vi, ViInt32 Chan);
ViStatus _VI_FUNC ML2430_GetProfileChannelSelection(ViSession vi, ViInt32 *Chan);
ViStatus _VI_FUNC ML2430_SetProfileConnectPoints(ViSession vi, ViInt32 ConPts);
ViStatus _VI_FUNC ML2430_GetProfileConnectPoints(ViSession vi, ViInt32 *ConPts);
ViStatus _VI_FUNC ML2430_SetProfileDataCollectionPeriod(ViSession vi, 
														ViReal64 CollPd);
ViStatus _VI_FUNC ML2430_GetProfileDataCollectionPeriod(ViSession vi, 
														ViReal64 *CollPd);
ViStatus _VI_FUNC ML2430_SetProfileExternalTriggerEdge(ViSession vi, 
													   ViInt32 Trig);
ViStatus _VI_FUNC ML2430_GetProfileExternalTriggerEdge(ViSession vi, 
													   ViInt32 *Trig);
ViStatus _VI_FUNC ML2430_SetProfileMinMaxTrackingMode(ViSession vi, 
													  ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetProfileMinMaxTrackingMode(ViSession vi, 
													   ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SetProfileReferenceLineState(ViSession vi, 
													  ViInt32 Lne);
ViStatus _VI_FUNC ML2430_GetProfileReferenceLineState(ViSession vi, 
													   ViInt32 *Lne);
ViStatus _VI_FUNC ML2430_ProfileReset(ViSession vi, ViInt32 Mode);
ViStatus _VI_FUNC ML2430_SetProfileTriggerArming(ViSession vi, ViInt32 St);
ViStatus _VI_FUNC ML2430_GetProfileTriggerArming(ViSession vi, ViInt32 *St);
ViStatus _VI_FUNC ML2430_SetProfileTriggerDelay(ViSession vi, ViReal64 Dly);
ViStatus _VI_FUNC ML2430_GetProfileTriggerDelay(ViSession vi, ViReal64 *Dly);
ViStatus _VI_FUNC ML2430_SetProfileTriggerGateWidth(ViSession vi, ViReal64 Wid);
ViStatus _VI_FUNC ML2430_GetProfileTriggerGateWidth(ViSession vi, ViReal64 *Wid);
ViStatus _VI_FUNC ML2430_SetProfileTriggerLevel(ViSession vi, ViReal64 Trig);
ViStatus _VI_FUNC ML2430_GetProfileTriggerLevel(ViSession vi, ViReal64 *Trig);
ViStatus _VI_FUNC ML2430_SetProfileTriggerSource(ViSession vi, ViInt32 Src);
ViStatus _VI_FUNC ML2430_GetProfileTriggerSource(ViSession vi, ViInt32 *Src);
ViStatus _VI_FUNC ML2430_SetProfileTriggerType(ViSession vi, ViInt32 Trg);
ViStatus _VI_FUNC ML2430_GetProfileTriggerType(ViSession vi, ViInt32 *Trg);
ViStatus _VI_FUNC ML2430_SetProfileType(ViSession vi, ViInt32 Typ);
ViStatus _VI_FUNC ML2430_GetProfileType(ViSession vi, ViInt32 *Typ);
ViStatus _VI_FUNC ML2430_SetProfileYAxisScale(ViSession vi, ViReal64 ULim,
											  ViReal64 LLim);
ViStatus _VI_FUNC ML2430_GetProfileYAxisScale(ViSession vi, ViReal64 *ULim,
											  ViReal64 *LLim);
ViStatus _VI_FUNC ML2430_RangeCalibratorDataQuery(ViSession vi, ViInt32 Snsr,
												  ViBoolean *St, 
												  ViInt32 *ZerVal, 
												  ViInt32 Up[5], ViInt32 Low[5]);
ViStatus _VI_FUNC ML2430_SetRedialCount(ViSession vi, ViUInt8 Rdl);
ViStatus _VI_FUNC ML2430_GetRedialCount(ViSession vi, ViUInt8 *Rdl);
ViStatus _VI_FUNC ML2430_SetRelativeControl(ViSession vi, ViInt32 Chan,
											ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetRelativeControl(ViSession vi, ViInt32 Chan,
											ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_ResetMinMaxGraph(ViSession vi);
ViStatus _VI_FUNC ML2430_Reset(ViSession vi);
ViStatus _VI_FUNC ML2430_ResetToFactory(ViSession vi);
ViStatus _VI_FUNC ML2430_SetRFRefCalibratorOnOff(ViSession vi, ViInt32 Cal);
ViStatus _VI_FUNC ML2430_GetRFRefCalibratorOnOff(ViSession vi, ViInt32 *Cal);
ViStatus _VI_FUNC ML2430_SetRS232BaudRate(ViSession vi, ViInt32 Rate);
ViStatus _VI_FUNC ML2430_GetRS232BaudRate(ViSession vi, ViInt32 *Rate);
ViStatus _VI_FUNC ML2430_SetRS232OperatingMode(ViSession vi, ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetRS232OperatingMode(ViSession vi, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SetSavedSetNaming(ViSession vi, ViUInt8 StNum,
										   ViChar Txt[]);
ViStatus _VI_FUNC ML2430_GetSavedSetNaming(ViSession vi, ViUInt8 StNum,
										   ViChar Txt[]);
ViStatus _VI_FUNC ML2430_LoadSavedSetup(ViSession vi, ViUInt16 StNum,
										ViChar Data[], ViInt32 Bytes);
ViStatus _VI_FUNC ML2430_ReadSavedSetup(ViSession vi, ViUInt16 StNum, ViInt32 DataSize,
										ViChar Data[], ViInt32 *Bytes);
ViStatus _VI_FUNC ML2430_SetSecureSystemState(ViSession vi, ViInt32 St);
ViStatus _VI_FUNC ML2430_GetSecureSystemState(ViSession vi, ViInt32 *St);
ViStatus _VI_FUNC ML2430_SetSelectSensorOffset(ViSession vi, ViInt32 Snsr,
											   ViInt32 Typ);
ViStatus _VI_FUNC ML2430_GetSelectSensorOffset(ViSession vi, ViInt32 Snsr,
											   ViInt32 *Typ);
ViStatus _VI_FUNC ML2430_SelfTestStatus(ViSession vi, ViChar Flsh[5], 
										ViChar CalDat[5], ViChar Prsnlty[5], 
										ViChar Ram[5], ViChar Nonvol[5], 
										ViChar Lcd[5], ViChar Kbd[5], 
										ViChar Dsp[5]);
ViStatus _VI_FUNC ML2430_SensorInformation(ViSession vi, ViInt32 Snsr,
										   ViInt32 InfoSize,ViChar Info[]);
ViStatus _VI_FUNC ML2430_SetSensorMeasurementMode(ViSession vi, ViInt32 Snsr,
											   	  ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetSensorMeasurementMode(ViSession vi, ViInt32 Snsr,
											   	  ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SensorOffsetValue(ViSession vi, ViInt32 Snsr,
										   ViReal64 *Val);
ViStatus _VI_FUNC ML2430_SetSensorRangeHold(ViSession vi, ViInt32 Snsr,
											ViUInt8 Tbl);
ViStatus _VI_FUNC ML2430_GetSensorRangeHold(ViSession vi, ViInt32 ISnsr,
											ViInt32 *OSnsr, ViUInt8 *Tbl);
ViStatus _VI_FUNC ML2430_SetSensorSettlePercentage(ViSession vi, ViInt32 Snsr,
												   ViReal64 Pct);
ViStatus _VI_FUNC ML2430_GetSensorSettlePercentage(ViSession vi, ViInt32 ISnsr,
												   ViInt32 *OSnsr,
												   ViReal64 *Pct);
ViStatus _VI_FUNC ML2430_SetQueryEventStatusRegister(ViSession vi,
													 ViUInt8 Inpt);
ViStatus _VI_FUNC ML2430_GetQueryEventStatusRegister(ViSession vi,
													 ViUInt8 *Inpt);
ViStatus _VI_FUNC ML2430_SetQuerySRQEnableRegister(ViSession vi, ViUInt8 Inpt);
ViStatus _VI_FUNC ML2430_GetQuerySRQEnableRegister(ViSession vi, ViUInt8 *Inpt);
ViStatus _VI_FUNC ML2430_SetupBNCAnalogOutput(ViSession vi, ViInt32 Prt,
											  ViReal64 StVolt, ViReal64 SpVolt,
											  ViInt32 StValUnit, ViReal64 StVal, 
											  ViInt32 SpValUnit, ViReal64 SpVal);
ViStatus _VI_FUNC ML2430_SetupCalFactorConfig(ViSession vi, ViInt32 Snsr,
											  ViInt32 Src, ViReal64 CalFrq, 
											  ViInt32 FrqUnit, ViReal64 AdjVal,
											  ViInt32 AdjUnit, ViReal64 ManVal,
											  ViInt32 ManUnit, ViInt32 Tbl);
ViStatus _VI_FUNC ML2430_SetupChannelConfig(ViSession vi, ViInt32 Chan,
											ViInt32 Cnfig, ViInt32 Trck, 
											ViInt32 Unit, ViInt32 Res);
ViStatus _VI_FUNC ML2430_SetupLimits(ViSession vi, ViInt32 Chan, ViReal64 HLim,
									 ViReal64 LLim, ViInt32 HLoo, ViInt32 LLoo, 
									 ViInt32 FlBp, ViInt32 FlIndHld);
ViStatus _VI_FUNC ML2430_SetupSensorAverage(ViSession vi, ViInt32 Snsr, 
											ViInt32 Typ, ViUInt16 NumRd, 
											ViInt32 Mode);
ViStatus _VI_FUNC ML2430_SetupSensorConfig(ViSession vi, ViInt32 Snsr, 
										   ViInt32 Mode, ViReal32 Pctg,
										   ViUInt8 Tbl);
ViStatus _VI_FUNC ML2430_SetupTriggerConfig(ViSession vi, ViInt32 RdOut, 
										    ViInt32 Chan, ViInt32 Src,
										    ViReal64 GtWid, ViReal64 DlyVal,
										    ViInt32 TrgArm, ViInt32 TrgTyp,
										    ViReal64 TrgLvl, ViInt32 ExTrgTyp);
ViStatus _VI_FUNC ML2430_SourceSweepModeStatusRequest(ViSession vi,
													  ViInt32 *Mode,
													  ViReal64 *StPow, 
													  ViReal64 *SpPow,
													  ViReal64 *StFrq,
													  ViReal64 *SpFrq);
ViStatus _VI_FUNC ML2430_SetSourceSweepMode(ViSession vi, ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetSourceSweepMode(ViSession vi, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SetSourceSweepStartFrequency(ViSession vi, 
													  ViReal64 Frq);
ViStatus _VI_FUNC ML2430_GetSourceSweepStartFrequency(ViSession vi, 
													  ViReal64 *Frq);
ViStatus _VI_FUNC ML2430_SetSourceSweepStartPower(ViSession vi, ViReal64 Pow);
ViStatus _VI_FUNC ML2430_GetSourceSweepStartPower(ViSession vi, ViReal64 *Pow);
ViStatus _VI_FUNC ML2430_SetSourceSweepStopFrequency(ViSession vi, 
													  ViReal64 Frq);
ViStatus _VI_FUNC ML2430_GetSourceSweepStopFrequency(ViSession vi, 
													  ViReal64 *Frq);
ViStatus _VI_FUNC ML2430_SetSourceSweepStopPower(ViSession vi, ViReal64 Pow);
ViStatus _VI_FUNC ML2430_GetSourceSweepStopPower(ViSession vi, ViReal64 *Pow);
ViStatus _VI_FUNC ML2430_Start(ViSession vi, ViInt32 *Sttus);
ViStatus _VI_FUNC ML2430_StatusMessage(ViSession vi, ViInt32 *OpMode,
									   struct GPIBStatus *GPIBSt,
									   struct ChannelStatus *ChanSt1,
									   struct ChannelStatus *ChanSt2,
									   struct SensorStatus *SnsrSt1,
									   struct SensorStatus *SnsrSt2);
ViStatus _VI_FUNC ML2430_SetSweepAveragingPoints(ViSession vi, ViInt32 Snsr,
												 ViInt16 Pts);
ViStatus _VI_FUNC ML2430_GetSweepAveragingPoints(ViSession vi, ViInt32 ISnsr,
												 ViInt32 *OSnsr, ViInt32 *Pts);
ViStatus _VI_FUNC ML2430_ResetSweepAveraging(ViSession vi);
ViStatus _VI_FUNC ML2430_SetSweepAveragingState(ViSession vi, ViInt32 SwpAvg);
ViStatus _VI_FUNC ML2430_GetSweepAveragingState(ViSession vi, ViInt32 *SwpAvg);
ViStatus _VI_FUNC ML2430_SetTextDisplay(ViSession vi, ViInt32 Txt);
ViStatus _VI_FUNC ML2430_GetTextDisplay(ViSession vi, ViInt32 *Txt);
ViStatus _VI_FUNC ML2430_SetText(ViSession vi, ViChar Txt[]);
ViStatus _VI_FUNC ML2430_GetText(ViSession vi, ViChar Txt[]);
ViStatus _VI_FUNC ML2430_TriggerAReading(ViSession vi, ViInt32 Trg, 
										 ViInt32 Chan, ViReal64 *RdRslt,
										 ViReal64 PflRslt[]);
ViStatus _VI_FUNC ML2430_SetTriggerArming(ViSession vi, ViInt32 Chan, 
										  ViInt32 Trg);
ViStatus _VI_FUNC ML2430_GetTriggerArming(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Trg);
ViStatus _VI_FUNC ML2430_SetTriggerDelay(ViSession vi, ViInt32 Chan, 
										 ViReal64 Dly);
ViStatus _VI_FUNC ML2430_GetTriggerDelay(ViSession vi, ViInt32 IChan, 
										 ViInt32 *OChan, ViReal64 *Dly);
ViStatus _VI_FUNC ML2430_SetTriggerExternalEdge(ViSession vi, ViInt32 Chan, 
										 		ViInt32 Trg);
ViStatus _VI_FUNC ML2430_GetTriggerExternalEdge(ViSession vi, ViInt32 IChan, 
										 		ViInt32 *OChan, ViInt32 *Trg);
ViStatus _VI_FUNC ML2430_SetTriggerGateWidth(ViSession vi, ViInt32 Chan, 
										 	 ViReal64 Trg);
ViStatus _VI_FUNC ML2430_GetTriggerGateWidth(ViSession vi, ViInt32 IChan, 
										 	 ViInt32 *OChan, ViReal64 *Trg);
ViStatus _VI_FUNC ML2430_SetTriggerLevel(ViSession vi, ViInt32 Chan, 
										 ViReal64 Trg);
ViStatus _VI_FUNC ML2430_GetTriggerLevel(ViSession vi, ViInt32 IChan, 
										 ViInt32 *OChan, ViReal64 *Trg);
ViStatus _VI_FUNC ML2430_SetTriggerLinking(ViSession vi, ViInt32 Lnk);
ViStatus _VI_FUNC ML2430_GetTriggerLinking(ViSession vi, ViInt32 *Lnk);
ViStatus _VI_FUNC ML2430_SetTriggerMode(ViSession vi, ViInt32 Mode);
ViStatus _VI_FUNC ML2430_GetTriggerMode(ViSession vi, ViInt32 *Mode);
ViStatus _VI_FUNC ML2430_SetTriggerSource(ViSession vi, ViInt32 Chan, 
										  ViInt32 Src);
ViStatus _VI_FUNC ML2430_GetTriggerSource(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Src);
ViStatus _VI_FUNC ML2430_SetTriggerType(ViSession vi, ViInt32 Chan, ViInt32 Typ);
ViStatus _VI_FUNC ML2430_GetTriggerType(ViSession vi, ViInt32 IChan, 
										ViInt32 *OChan, ViInt32 *Typ);
ViStatus _VI_FUNC ML2430_ZeroAnalogInput(ViSession vi);
ViStatus _VI_FUNC ML2430_ZeroSensor(ViSession vi, ViInt32 Snsr);
ViStatus _VI_FUNC ML2430_FloatToFourBytes(ViReal64 Flt, ViChar Bytes[4]);
ViStatus _VI_FUNC ML2430_FourBytesToFloat(ViChar Bytes[4], ViReal64 *Flt);

ViStatus _VI_FUNC ML2430_SaveInstrumentSetup(ViSession vi, ViInt32 Num);
ViStatus _VI_FUNC ML2430_RecallInstrumentSetup(ViSession vi, ViInt32 Num);



#endif
