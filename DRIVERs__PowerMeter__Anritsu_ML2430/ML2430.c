/*****************************************************************************/
/*****************************************************************************/
/**			                                               	                **/
/**			                 ANRITSU ML2430 DRIVERS      	                **/
/**			                                               	                **/
/*****************************************************************************/
/*****************************************************************************/

#include "ML2430.h"

/* Data type definitions */

#define max(a,b) (a > b ? a : b)

typedef union Data16Union
{
	ViInt16 IVal;
	ViChar CVal[2];
} Data16;

typedef union Data32Union
{
	ViInt32 LVal;
	ViReal32 FVal;
	ViChar CVal[4];
} Data32;


/****************************************************************************/
/*							 INITIALIZE FUNCTION							*/
/*					          												*/
/*	Initializes the vi instrument session.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_Initialize( ViChar *Resource , ViInt32 Rst, ViSession *vi)
{
	ViStatus error;
	ViSession RsrcMgr = 0;
	
	checkErr(viOpenDefaultRM(&RsrcMgr));
	
	checkErr(viOpen(RsrcMgr, Resource, VI_NULL, 10000, vi));
	viSetAttribute(*vi, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viSetAttribute(*vi, VI_ATTR_TERMCHAR, 10);
	if(Rst==Switch_On)
		ML2430_Reset(*vi);
	checkErr(viPrintf(*vi, "*CLS\n"));

Error:
	return error;
}

/****************************************************************************/
/*							    CLOSE FUNCTION	    						*/
/*					          												*/
/*	Closes the vi instrument session.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_Close(ViSession *vi)
{
	return viClose(*vi);
}

/****************************************************************************/
/*							    New Read FUNCTION	    					*/
/*					          												*/
/*	The Instrument sometimes does not respond to a query command and times  */
/*    out, but if you then send another read the response is there.         */
/*																			*/
/****************************************************************************/

/*ViStatus _VI_FUNC ML2430_Read(ViSession vi, ViChar *Buf, ViInt32 *BufSize2 )
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[1000];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;

	viRead(vi, Buf, BufSize, &BufSize);
	if (BufSize == 0)
	 	checkErr(viRead(vi, Buf, BufSize, &BufSize));
 	ReadBuf[BufSize]=0x00;
 	*BufSize2 = BufSize;
Error:
	return error;
}

*/

/****************************************************************************/
/*					  AC MOD OUTPUT CONFIG SET FUNCTION    					*/
/*					          												*/
/*	Sets the AC polarity.													*/
/*																			*/
/****************************************************************************/
									 //enum Polarity
ViStatus _VI_FUNC ML2430_SetACModOutputConfig(ViSession vi, ViInt32 Plrty)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][6] = {"NEG\n", "POS\n"};
	checkErr(viPrintf(vi, "OBACM %s", Matrix[Plrty])); 
	
Error:
	return error;
}

/****************************************************************************/
/*				      AC MOD OUTPUT CONFIG GET FUNCTION	  					*/
/*					          												*/
/*	Returns the AC polarity.												*/
/*																			*/
/****************************************************************************/
									 
ViStatus _VI_FUNC ML2430_GetACModOutputConfig(ViSession vi, ViInt32 *Plrty)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "OBACM?\n")); 
 	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
 	ReadBuf[BufSize]=0x00;

	*Plrty = PosPolarity;
	if(strstr(ReadBuf, "POS")==NULL)
	    *Plrty = NegPolarity;
	
Error:
	return error;
} 

/****************************************************************************/
/*					  		AUTO SCALING FUNCTION    						*/
/*					          												*/
/*	Sets auto-scaling.														*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_AutoScaling(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GRAUTO\n")); 
	
Error:
	return error;
}

/****************************************************************************/
/*				  AUTODIAL ENABLE FOR POWER ON SET FUNCTION    				*/
/*					          												*/
/*	Sets the power-on autodial on or off.									*/
/*																			*/
/****************************************************************************/
								 //enum AutoDial
ViStatus _VI_FUNC ML2430_SetAutodialEnableForPower(ViSession vi, ViInt32 AutoD)
{
	ViStatus error = VI_SUCCESS;
	if(AutoD==AD_Disabled)
		checkErr(viPrintf(vi, "MODPWR FALSE\n")); 
	else
		checkErr(viPrintf(vi, "MODPWR TRUE\n")); 
	
Error:
	return error;
}

/****************************************************************************/
/*				  AUTODIAL ENABLE FOR POWER ON GET FUNCTION	 				*/
/*					          												*/
/*	Returns whether the power-on autodial is on or off.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetAutodialEnableForPower(ViSession vi, ViInt32 *AutoD)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "MODPWR?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;
	
	*AutoD = AD_Enabled;
	if(strstr(ReadBuf, "TRUE")==NULL)
	    *AutoD = AD_Disabled;
	
Error:
	return error;
} 

/****************************************************************************/
/*				 AUTODIAL ENABLE FOR RANGE ERROR SET FUNCTION   			*/
/*					          												*/
/*	Sets the range-error autodial on or off.								*/
/*																			*/
/****************************************************************************/
								   //enum AutoDial
ViStatus _VI_FUNC ML2430_SetAutodialEnableForRange(ViSession vi, ViInt32 AutoD)
{
	ViStatus error = VI_SUCCESS;
	if(AutoD==AD_Disabled)
		checkErr(viPrintf(vi, "MODRNG FALSE\n")); 
	else
		checkErr(viPrintf(vi, "MODRNG TRUE\n")); 
	
Error:
	return error;
}

/****************************************************************************/
/*			     AUTODIAL ENABLE FOR RANGE ERROR GET FUNCTION 				*/
/*					          												*/
/*	Returns whether the range-error autodial is on or off.					*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetAutodialEnableForRange(ViSession vi, ViInt32 *AutoD)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "MODRNG?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

    *AutoD = AD_Enabled;
    if(strstr(ReadBuf, "TRUE")==NULL)
	    *AutoD = AD_Disabled;
	
Error:
	return error;
}

/****************************************************************************/
/*				          AUTODIAL ENABLE SET FUNCTION          			*/
/*					          												*/
/*	Sets the autodial on or off.											*/
/*																			*/
/****************************************************************************/
								   //enum AutoDial
ViStatus _VI_FUNC ML2430_SetAutodialEnable(ViSession vi, ViInt32 AutoD)
{
	ViStatus error = VI_SUCCESS;
	if(AutoD==AD_Disabled)
		checkErr(viPrintf(vi, "MODLIM FALSE\n")); 
	else
		checkErr(viPrintf(vi, "MODLIM TRUE\n")); 
	
Error:
	return error;
}

/****************************************************************************/
/*			     		  AUTODIAL ENABLE GET FUNCTION 						*/
/*					          												*/
/*	Returns whether the autodial is on or off.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetAutodialEnable(ViSession vi, ViInt32 *AutoD)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "MODLIM?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*AutoD = AD_Enabled;
	if(strstr(ReadBuf, "TRUE")==NULL)
	    *AutoD = AD_Disabled;
	
Error:
	return error;
}

/****************************************************************************/
/*				       AUTODIAL PHONE NUMBER SET FUNCTION          			*/
/*					          												*/
/*	Sets the autodial phone number.											*/
/*																			*/
/****************************************************************************/
								   
ViStatus _VI_FUNC ML2430_SetAutodialPhoneNumber(ViSession vi, ViChar Strng[])
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "MODPH %s\n", Strng)); 
	
Error:
	return error;
}

/****************************************************************************/
/*			     	   AUTODIAL PHONE NUMBER GET FUNCTION 					*/
/*					          												*/
/*	Returns the autodial phone number.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetAutodialPhoneNumber(ViSession vi, ViInt32 MaxLngth, 
										   		ViChar Strng[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize=sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "MODPH?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	checkErr(Scan(Buf, "MODPH %s[w*t32q]", MaxLngth, Strng)); 
	
Error:
	return error;
}

/****************************************************************************/
/*				     AVERAGING AUTO LOW LEVEL SET FUNCTION         			*/
/*					          												*/
/*	Sets the low level averaging window.									*/
/*																			*/
/****************************************************************************/
								   
ViStatus _VI_FUNC ML2430_SetAveragingAutoLowLevel(ViSession vi, ViInt32 Snsr,
												  ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][7] = {"OFF", "LOW", "MEDIUM", "HIGH"};
	checkErr(viPrintf(vi, "AVGLL %s,%s\n", Temp[Snsr], Matrix[Mode])); 
	
Error:
	return error;
}

/****************************************************************************/
/*			     	 AVERAGING AUTO LOW LEVEL GET FUNCTION 					*/
/*					          												*/
/*	Returns the low level averaging window.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetAveragingAutoLowLevel(ViSession vi, ViInt32 ISnsr, 
												  ViInt32 *OSnsr, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100];
	ViInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "AVGLL? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(ReadBuf, "A,")==NULL)
	    *OSnsr = SensorB;
	*Mode = Mode_Off;
	if(strstr(ReadBuf, "OFF\n")==NULL)
		if(strstr(ReadBuf, "LOW\n")==NULL)
			if(strstr(ReadBuf, "MEDIUM\n")==NULL)
				*Mode = Mode_High;
			else
				*Mode = Mode_Medium;
		else
			*Mode = Mode_Low;
	
Error:
	return error;
}

/****************************************************************************/
/*			     	 	  AVERAGING MOVING FUNCTION 						*/
/*					          												*/
/*	Sets the averaging type to moving from auto.							*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_AveragingMoving(ViSession vi, ViInt32 AvgType)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "AVGM %s\n", Temp[AvgType])); 
		
Error:
	return error;
}

/****************************************************************************/
/*				     	 AVERAGING SETUP SET FUNCTION  		       			*/
/*					          												*/
/*	Sets up averaging for a sensor.											*/
/*																			*/
/****************************************************************************/
								   
ViStatus _VI_FUNC ML2430_SetAveragingSetup(ViSession vi, ViInt32 Snsr, 
										   ViInt32 AvgType, ViUInt16 NumRds)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][4] = {"OFF", "MOV", "RPT", "AUTO"};
	checkErr(viPrintf(vi, "AVG %s,%s,%d\n", Temp[Snsr], Matrix[AvgType], 
			 		  NumRds)); 	
	
Error:
	return error;
}

/****************************************************************************/
/*			     	 	  AVERAGING SETUP GET FUNCTION 						*/
/*					          												*/
/*	Returns the averaging setup for a sensor.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetAveragingSetup(ViSession vi, ViInt32 ISnsr, 
										   ViInt32 *OSnsr, ViInt32 *AvgType, 
										   ViInt32 *NumRds)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100], Tmp[20];
	ViInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "AVG? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(ReadBuf, "A,")==NULL)
	    *OSnsr = SensorB;
	*AvgType = TypeMode_Off;
	if(strstr(ReadBuf, "OFF,")==NULL)
		if(strstr(ReadBuf, "MOV,")==NULL)
			if(strstr(ReadBuf, "RPT,")==NULL)
				*AvgType = TypeMode_Auto;
			else
				*AvgType = TypeMode_Rpt;
		else
			*AvgType = TypeMode_Mov;
	Scan(ReadBuf, "%s[w*t32q] %s[w*t44q],%s[w*t44q],%d", 19, Tmp, 19, Tmp, 19,
		 Tmp, NumRds);
	
Error:
	return error;
}

/****************************************************************************/
/*			     	 AVG BETWEEN PROFILE CURSORS FUNCTION 					*/
/*					          												*/
/*	Sets the averaging on or off.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_AvgBetweenProfileCursors(ViSession vi, ViInt32 Avging)
{
	ViStatus error = VI_SUCCESS;
	if(Avging==Switch_Off)
	    checkErr(viPrintf(vi, "GRCAVG OFF\n")); 
	else
	    checkErr(viPrintf(vi, "GRCAVG ON\n")); 

Error:
	return error;
}

/****************************************************************************/
/*				         BACK LIGHT TIMER SET FUNCTION           			*/
/*					          												*/
/*	Sets the back light timer.												*/
/*																			*/
/****************************************************************************/
								   
ViStatus _VI_FUNC ML2430_SetBackLightTimer(ViSession vi, ViReal64 Tme)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "DBLTIM %f\n", Tme)); 
	
Error:
	return error;
}

/****************************************************************************/
/*			     	     BACK LIGHT TIMER GET FUNCTION 	    				*/
/*					          												*/
/*	Returns the back light timer.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBackLightTimer(ViSession vi, ViReal64 *Tme)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "DBLTIM?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	checkErr(Scan(Buf, "DBLTIM %f", Tme)); 
	
Error:
	return error;
}

/****************************************************************************/
/*				  BATTERY AUTO TURN OFF MINUTES SET FUNCTION       			*/
/*					          												*/
/*	Sets the battery auto turn off minutes.									*/
/*																			*/
/****************************************************************************/
								   
ViStatus _VI_FUNC ML2430_SetBatteryAutoTurnOffMinutes(ViSession vi, ViUInt8 Min)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "BAUTT %d\n", Min)); 
	
Error:
	return error;
}

/****************************************************************************/
/*			      BATTERY AUTO TURN OFF MINUTES GET FUNCTION 	  			*/
/*					          												*/
/*	Returns the battery auto turn off minutes.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBatteryAutoTurnOffMinutes(ViSession vi,ViUInt8 *Min)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "BAUTT?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	checkErr(Scan(Buf, "BAUTT %d", &Tmp));
	*Min = Tmp;
	
Error:
	return error;
}

/****************************************************************************/
/*				  	  BATTERY AUTO TURN OFF SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the battery auto turn off to disabled or enabled.					*/
/*																			*/
/****************************************************************************/
								   
ViStatus _VI_FUNC ML2430_SetBatteryAutoTurnOff(ViSession vi, ViInt32 AutoOff)
{
	ViStatus error = VI_SUCCESS;
	if(AutoOff==BA_Disable)
		checkErr(viPrintf(vi, "BAUTS DISABLE\n")); 
	else
		checkErr(viPrintf(vi, "BAUTS ENABLE\n")); 

Error:
	return error;
}

/****************************************************************************/
/*			          BATTERY AUTO TURN OFF GET FUNCTION 	    			*/
/*					          												*/
/*	Returns whether the battery auto turn off is set to disabled or enabled.*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBatteryAutoTurnOff(ViSession vi, ViInt32 *AutoOff)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "BAUTS?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*AutoOff = BA_Enable;
	if(strstr(ReadBuf, "ENABLE")==NULL)
	    *AutoOff = BA_Disable;
	
Error:
	return error;
}

/****************************************************************************/
/*				  BATTERY LCD BACK LIGHT MODE SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the battery LCD back light mode.									*/
/*																			*/
/****************************************************************************/
								   	   //enum BatteryLCDMode
ViStatus _VI_FUNC ML2430_SetBatteryLCDBackLightMode(ViSession vi, ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[3][6] = {"ON", "OFF", "TIMED"};
	checkErr(viPrintf(vi, "DBLGHT %s\n", Matrix[Mode])); 

Error:
	return error;
}

/****************************************************************************/
/*			      BATTERY LCD BACK LIGHT MODE GET FUNCTION 	    			*/
/*					          												*/
/*	Returns the battery LCD back light mode.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBatteryLCDBackLightMode(ViSession vi, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "DBLGHT?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*Mode = LCD_On;
	if(strstr(ReadBuf, "DBLGHT ON\n")==NULL)
	    if(strstr(ReadBuf, "DBLGHT OFF\n")==NULL)
	    	*Mode = LCD_Timed;
	    else
	    	*Mode = LCD_Off;

Error:
	return error;
}

/****************************************************************************/
/*				  	 BLANKING INPUT POLARITY SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the blanking polarity.												*/
/*																			*/
/****************************************************************************/
								      //enum Polarity
ViStatus _VI_FUNC ML2430_SetBlankingInputPolarity(ViSession vi, ViInt32 Plrty)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"NEG", "POS"};
	
	checkErr(viPrintf(vi, "IBBLP %s\n", Matrix[Plrty])); 

Error:
	return error;
}

/****************************************************************************/
/*			         BLANKING INPUT POLARITY GET FUNCTION 	    			*/
/*					          												*/
/*	Returns the blanking polarity.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBlankingInputPolarity(ViSession vi, ViInt32 *Plrty)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "IBBLP?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*Plrty = PosPolarity;
	if(strstr(ReadBuf, "POS")==NULL)
	    *Plrty = NegPolarity;
	
Error:
	return error;
}

/****************************************************************************/
/*			   BNC ANALOG OUT DISPLAY START VALUE SET FUNCTION   	  		*/
/*					          												*/
/*	Sets the BNC analog display start value.								*/
/*																			*/
/****************************************************************************/
								      //enum ChannelSelect, BNCUnits
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutDisplayStart(ViSession vi, ViInt32 Chan, 
													 ViInt32 Unit, ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[5][5] = {"W", "DB", "DBM", "DBUV", "DBMV"};
	
	checkErr(viPrintf(vi, "OBDST %d,%s,%f\n", (Chan+1), Matrix[Unit], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			   BNC ANALOG OUT DISPLAY START VALUE GET FUNCTION 	  			*/
/*					          												*/
/*	Returns the BNC analog display start value.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCAnalogOutDisplayStart(ViSession vi, 
													 ViInt32 IChan,
													 ViInt32 *OChan,
													 ViInt32 *Unit, 
													 ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100], Tmp[20];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	ViInt32 TmpInt;
	checkErr(viPrintf(vi, "OBDST? %d\n", (IChan+1))); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	Scan(ReadBuf, "OBDST %d,", &TmpInt);
	*OChan = TmpInt-1;
	*Unit = BNC_W;
	if(strstr(ReadBuf, "W,")==NULL)
	    if(strstr(ReadBuf, "DBMV,")==NULL)
	    	if(strstr(ReadBuf, "DBUV,")==NULL)
	    		if(strstr(ReadBuf, "DBM,")==NULL)
	    			*Unit = BNC_DB;
	    		else
	    			*Unit = BNC_DBM;
	    	else
	    		*Unit = BNC_DBUV;
	    else
	    	*Unit = BNC_DBMV;
	Scan(ReadBuf, "%s[w*t44q],%s[w*t44q],%f", 19, Tmp, 19, Tmp, Val);

Error:
	return error;
}

/****************************************************************************/
/*			    BNC ANALOG OUT DISPLAY STOP VALUE SET FUNCTION   	  		*/
/*					          												*/
/*	Sets the BNC analog display stop value.	    							*/
/*																			*/
/****************************************************************************/
								      //enum ChannelSelect, BNCUnits
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutDisplayStop(ViSession vi, ViInt32 Chan, 
													ViInt32 Unit, ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[5][5] = {"W", "DB", "DBM", "DBUV", "DBMV"};
	
	checkErr(viPrintf(vi, "OBDSP %d,%s,%f\n", (Chan+1), Matrix[Unit], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			    BNC ANALOG OUT DISPLAY STOP VALUE GET FUNCTION 	  			*/
/*					          												*/
/*	Returns the BNC analog display stop value.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCAnalogOutDisplayStop(ViSession vi, ViInt32 IChan,
													ViInt32 *OChan, 
													ViInt32 *Unit,ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100], Tmp[20];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	ViInt32 TmpInt;
	checkErr(viPrintf(vi, "OBDSP? %d\n", (IChan+1))); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	Scan(ReadBuf, "OBDSP %d,", &TmpInt);
	*OChan = TmpInt-1;
	*Unit = BNC_W;
	if(strstr(ReadBuf, "W,")==NULL)
	    if(strstr(ReadBuf, "DBUV,")==NULL)
	    	if(strstr(ReadBuf, "DBMV,")==NULL)
	    		if(strstr(ReadBuf, "DBM,")==NULL)
	    			*Unit = BNC_DB;
	    		else
	    			*Unit = BNC_DBM;
	    	else
	    		*Unit = BNC_DBMV;
	    else
	    	*Unit = BNC_DBUV;
	Scan(ReadBuf, "%s[w*t44q],%s[w*t44q],%f", 19, Tmp, 19, Tmp, Val);
	
Error:
	return error;
}

/****************************************************************************/
/*			   BNC ANALOG OUTPUT START VOLTAGE SCALE SET FUNCTION    		*/
/*					          												*/
/*	Sets the BNC analog output start voltage scale.							*/
/*																			*/
/****************************************************************************/
								      //enum PortSelect
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutputStartVolt(ViSession vi, ViInt32 Port, 
													 ViReal64 Strt)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "OBVST %d,%f\n", (Port+1), Strt)); 

Error:
	return error;
}

/****************************************************************************/
/*			   BNC ANALOG OUTPUT START VOLTAGE SCALE GET FUNCTION 	  		*/
/*					          												*/
/*	Returns the BNC analog output start voltage scale.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCAnalogOutputStartVolt(ViSession vi,
													 ViInt32 IPort,
													 ViInt32 *OPort, 
													 ViReal64 *Strt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 TmpInt, BufSize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "OBVST? %d\n", (IPort+1))); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	checkErr(Scan(Buf, "OBVST %d,%f", &TmpInt, Strt));
	*OPort = TmpInt-1;
	
Error:
	return error;
}

/****************************************************************************/
/*			   BNC ANALOG OUTPUT STOP VOLTAGE SCALE SET FUNCTION    		*/
/*					          												*/
/*	Sets the BNC analog output stop voltage scale.							*/
/*																			*/
/****************************************************************************/
								      
ViStatus _VI_FUNC ML2430_SetBNCAnalogOutputStopVolt(ViSession vi, ViInt32 Port, 
													ViReal64 Stp)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "OBVSP %d,%f\n", (Port+1), Stp)); 

Error:
	return error;
}

/****************************************************************************/
/*			   BNC ANALOG OUTPUT STOP VOLTAGE SCALE GET FUNCTION 	  		*/
/*					          												*/
/*	Returns the BNC analog output stop voltage scale.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCAnalogOutputStopVolt(ViSession vi, ViInt32 IPort, 
													ViInt32 *OPort, 
													ViReal64 *Stp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 TmpInt, BufSize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "OBVSP? %d\n", (IPort+1)));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	checkErr(Scan(Buf, "OBVSP %d,%f", &TmpInt, Stp));
	*OPort = TmpInt-1;
	
Error:
	return error;
}

/****************************************************************************/
/*			       BNC OUT PORT CHANNEL CONFIG SET FUNCTION         		*/
/*					          												*/
/*	Sets the BNC out port channel configuration.							*/
/*																			*/
/****************************************************************************/
								      
ViStatus _VI_FUNC ML2430_SetBNCOutPortChannel(ViSession vi, ViInt32 Port, 
											  ViInt32 Chan)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "OBCH %d,%d\n", (Port+1), (Chan+1))); 

Error:
	return error;
}

/****************************************************************************/
/*			       BNC OUT PORT CHANNEL CONFIG GET FUNCTION 	    		*/
/*					          												*/
/*	Returns the BNC out port channel configuration.							*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCOutPortChannel(ViSession vi, ViInt32 Port, 
											  ViInt32 *Chan)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100], TmpBuf[20];
	ViInt32 Tmp, BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "OBCH? %d\n", (Port+1)));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	checkErr(Scan(Buf, "%s[w*t44q],%d", 19, TmpBuf, &Tmp)); 
	*Chan = Tmp-1;
	
Error:
	return error;
}

/****************************************************************************/
/*			       		BNC OUTPUT MODE SET FUNCTION 		        		*/
/*					          												*/
/*	Sets the BNC output mode.												*/
/*																			*/
/****************************************************************************/
								      
ViStatus _VI_FUNC ML2430_SetBNCOutputMode(ViSession vi, ViInt32 Chan,
										  ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[11][10] = {"OFF","AOUT","PASS/FAIL","SIGA","LVLA1","LVLA2",
							 "LVLB1","LVLB2","ACMOD","RFB","SIGB"};
	checkErr(viPrintf(vi, "OBMD %d,%s\n", (Chan+1), Matrix[Mode])); 

Error:
	return error;
}

/****************************************************************************/
/*			       		BNC OUTPUT MODE GET FUNCTION 	  			  		*/
/*					          												*/
/*	Returns the BNC output mode.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCOutputMode(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1, TmpInt;
	checkErr(viPrintf(vi, "OBMD? %d\n", (IChan+1))); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	Scan(Buf, "OBMD %d,", &TmpInt);
	*OChan = TmpInt-1;
	*Mode = BNCMode_OFF;
	if(strstr(Buf, "OFF\n")==NULL)
		if(strstr(Buf, "AOUT\n")==NULL)
			if(strstr(Buf, "PASS/FAIL\n")==NULL)
				if(strstr(Buf, "SIGA\n")==NULL)
					if(strstr(Buf, "LVLA1\n")==NULL)
						if(strstr(Buf, "LVLA2\n")==NULL)
							if(strstr(Buf, "LVLB1\n")==NULL)
								if(strstr(Buf, "LVLB2\n")==NULL)
									if(strstr(Buf, "ACMOD\n")==NULL)
										if(strstr(Buf, "RFB\n")==NULL)
											*Mode = BNCMode_SIGB;
										else
											*Mode = BNCMode_RFB;
									else
										*Mode = BNCMode_ACMOD;
								else
									*Mode = BNCMode_LVLB2;
							else
								*Mode = BNCMode_LVLB1;
						else
							*Mode = BNCMode_LVLA2;
					else
						*Mode = BNCMode_LVLA1;
				else
					*Mode = BNCMode_SIGA;
			else
				*Mode = BNCMode_PASS_FAIL;
		else
			*Mode = BNCMode_AOUT;
		
Error:
	return error;
}

/****************************************************************************/
/*			       	BNC PASS-FAIL PASS LEVEL SET FUNCTION 	        		*/
/*					          												*/
/*	Sets the BNC pass-fail pass level.										*/
/*																			*/
/****************************************************************************/
								        //enum HighLowLevel
ViStatus _VI_FUNC ML2430_SetBNCPassLevel(ViSession vi, ViInt32 Port, 
										 ViInt32 Lvl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][5] = {"HIGH","LOW"};
	checkErr(viPrintf(vi, "OBPL %d,%s\n", (Port+1), Matrix[Lvl])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	BNC PASS-FAIL PASS LEVEL GET FUNCTION 	  		  		*/
/*					          												*/
/*	Returns the BNC pass-fail pass level.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCPassLevel(ViSession vi, ViInt32 Port, 
										 ViInt32 *Lvl)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "OBPL? %d\n", (Port+1))); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*Lvl = PFL_High;
	if(strstr(ReadBuf, "HIGH")==NULL)
	    *Lvl = PFL_Low;

Error:
	return error;
}

/****************************************************************************/
/*			       	     BNC RF BLANKING SET FUNCTION 	            		*/
/*					          												*/
/*	Sets the BNC RF blanking.												*/
/*																			*/
/****************************************************************************/
								 //enum HighLowLevel
ViStatus _VI_FUNC ML2430_SetBNCRFBlanking(ViSession vi, ViInt32 Lvl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][5] = {"HIGH","LOW"};
	checkErr(viPrintf(vi, "OBZL %s\n", Matrix[Lvl])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	     BNC RF BLANKING GET FUNCTION 	     		  		*/
/*					          												*/
/*	Returns the BNC RF blanking.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetBNCRFBlanking(ViSession vi, ViInt32 *Lvl)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "OBZL?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	*Lvl = PFL_High;
	if(strstr(ReadBuf, "HIGH")==NULL)
	    *Lvl = PFL_Low;

Error:
	return error;
}

/****************************************************************************/
/*			       	         CAL ADD ENTRY FUNCTION 	     		  		*/
/*					          												*/
/*	Adds an entry pair to a cal factor table.								*/
/*																			*/
/****************************************************************************/
										//enum CalUnits
ViStatus _VI_FUNC ML2430_CalAddEntry(ViSession vi, ViInt32 Snsr, ViUInt8 Tbl,
									 ViReal64 Frq, ViReal64 Val, ViInt32 Unit)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][4] = {"%", "PCT", "DB", "DBM"};
	checkErr(viPrintf(vi, "CFUADD %s,%d,%f,%f,%s\n", Temp[Snsr], Tbl, Frq, Val, 
					  Matrix[Unit])); 
											 //%f was %d ??
Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL ADJUST SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the calibration factor.											*/
/*																			*/
/****************************************************************************/
									   //enum CalUnits
ViStatus _VI_FUNC ML2430_SetCalAdjust(ViSession vi, ViInt32 Snsr, ViInt32 Unit, 
									  ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][4] = {"%", "PCT", "DB", "DBM"};
	checkErr(viPrintf(vi, "CFADJ %s,%s,%f\n", Temp[Snsr], Matrix[Unit], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL ADJUST GET FUNCTION         		  		*/
/*					          												*/
/*	Returns the calibration factor.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalAdjust(ViSession vi, ViInt32 ISnsr, 
									  ViInt32 *OSnsr, ViInt32 *Unit, 
									  ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[100], Tmp[20];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "CFADJ? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(Buf, "A,")==NULL)
		*OSnsr = SensorB;
	*Unit = Cal_Percent;
	if(strstr(Buf, "%,")==NULL)
		*Unit = Cal_DB;
	Scan(Buf, "%s[w*t32q] %s[w*t44q],%s[w*t44q],%f", 19, Tmp, 19, Tmp, 19, Tmp, 
		 Val);
	
Error:
	return error;
}

/****************************************************************************/
/*			       	      CAL CURRENT VALUE FUNCTION         		  		*/
/*					          												*/
/*	Returns the currently used cal factor value.							*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_CalCurrentValue(ViSession vi, ViInt32 Snsr, 
										 ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CFVAL %s\n", Temp[Snsr])); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%f", Val));
	
Error:
	return error;
}

/****************************************************************************/
/*			       	    CAL DISPLAY UNITS SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the cal units display. 											*/
/*																			*/
/****************************************************************************/
									   //enum CalUnits
ViStatus _VI_FUNC ML2430_SetCalDisplayUnits(ViSession vi, ViInt32 Snsr, 
											ViInt32 Unit)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][4] = {"%", "PCT", "DB", "DBM"};
	checkErr(viPrintf(vi, "CFUNITS %s,%s\n", Temp[Snsr], Matrix[Unit])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	    CAL DISPLAY UNITS GET FUNCTION         		  		*/
/*					          												*/
/*	Returns the cal units display.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalDisplayUnits(ViSession vi, ViInt32 ISnsr, 
											ViInt32 *OSnsr, ViInt32 *Unit)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "CFUNITS? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(ReadBuf, "A")==NULL)
	    *OSnsr = SensorB;
	*Unit = Cal_DBM;
	if(strstr(ReadBuf, "DBM")==NULL)
	    *Unit = Cal_PCT;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	      CAL FREQUENCY SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the cal frequency.     											*/
/*																			*/
/****************************************************************************/
									      //enum CalFrequency
ViStatus _VI_FUNC ML2430_SetCalFrequency(ViSession vi, ViInt32 Snsr, 
										 ViInt32 Unit, ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][4] = {"HZ", "KHZ", "MHZ", "GHZ"};
	checkErr(viPrintf(vi, "CFFRQ %s,%f%s\n", Temp[Snsr], Val, Matrix[Unit])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	      CAL FREQUENCY GET FUNCTION         		  		*/
/*					          												*/
/*	Returns the cal frequency.	    										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalFrequency(ViSession vi, ViInt32 ISnsr, 
										 ViInt32 *OSnsr, ViInt32 *Unit, 
										 ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar TmpChr[3], Buf[100], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CFFRQ? %s\n", Temp[ISnsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f", 10, Tmp, 2, TmpChr, Val));
	
	if ( OSnsr )
	{
		*OSnsr = SensorA;
		if(TmpChr[0]=='B')
			*OSnsr = SensorB;
	}
	
	if ( Unit )
	{
		*Unit = Frq_Hz;	
	}
	
Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL MANUAL SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the cal factor number.												*/
/*																			*/
/****************************************************************************/
									   //enum CalUnits
ViStatus _VI_FUNC ML2430_SetCalManual(ViSession vi, ViInt32 Snsr, ViInt32 Unit, 
									  ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[4][4] = {"%", "PCT", "DB", "DBM"};
	checkErr(viPrintf(vi, "CFCAL %s,%s,%f\n", Temp[Snsr], Matrix[Unit], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL MANUAL GET FUNCTION         		  		*/
/*					          												*/
/*	Returns the cal factor number.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalManual(ViSession vi, ViInt32 ISnsr, 
									  ViInt32 *OSnsr, ViInt32 *Unit, 
									  ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100], Tmp[20];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "CFCAL? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(ReadBuf, "A,")==NULL)
	    *OSnsr = SensorB;
	*Unit = Cal_Percent;
	if(strstr(ReadBuf, "%,")==NULL)
	    *Unit = Cal_DB;
	Scan(ReadBuf, "%s[w*t32q] %s[w*t44q],%s[w*t44q],%f", 19, Tmp, 19, Tmp, 19,
		 Tmp, Val);
	
Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL SOURCE SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the cal source.  													*/
/*																			*/
/****************************************************************************/
									   //enum CalSource
ViStatus _VI_FUNC ML2430_SetCalSource(ViSession vi, ViInt32 Snsr, ViInt32 Src)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[3][5] = {"FREQ", "MAN", "VGHZ"};
	checkErr(viPrintf(vi, "CFSRC %s,%s\n", Temp[Snsr], Matrix[Src])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL SOURCE GET FUNCTION         		  		*/
/*					          												*/
/*	Returns the cal source.    												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalSource(ViSession vi, ViInt32 ISnsr, 
									  ViInt32 *OSnsr, ViInt32 *Src)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "CFSRC? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(ReadBuf, "A,")==NULL)
	    *OSnsr = SensorB;
	*Src = Src_FREQ;
	if(strstr(ReadBuf, "FREQ\n")==NULL)
		if(strstr(ReadBuf, "MAN\n")==NULL)
	    	*Src = Src_VGHZ;
	    else
	    	*Src = Src_MAN;

Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL START FREQUENCY SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the cal start frequency.   										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetCalStartFrequency(ViSession vi, ViInt32 Snsr, 
										 	  ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CVSTF %s,%f\n", Temp[Snsr], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL START FREQUENCY GET FUNCTION         	  		*/
/*					          												*/
/*	Returns the cal start frequency.   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalStartFrequency(ViSession vi, ViInt32 Snsr, 
										 	  ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[20], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CVSTF? %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f", 10, Tmp, 10, Tmp, Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	    CAL START VOLTAGE SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the cal start voltage.	 	  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetCalStartVoltage(ViSession vi, ViInt32 Snsr, 
										 	ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CVSTV %s,%f\n", Temp[Snsr], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	    CAL START VOLTAGE GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the cal start voltage.   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalStartVoltage(ViSession vi, ViInt32 Snsr, 
										 	ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[20], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CVSTV? %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f", 10, Tmp, 10, Tmp, Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL STOP FREQUENCY SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the cal stop frequency.    										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetCalStopFrequency(ViSession vi, ViInt32 Snsr, 
										 	 ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CVSPF %s,%f\n", Temp[Snsr], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL STOP FREQUENCY GET FUNCTION          	  		*/
/*					          												*/
/*	Returns the cal stop frequency.   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalStopFrequency(ViSession vi, ViInt32 Snsr, 
										 	 ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[20], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CVSPF? %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f", 10, Tmp, 10, Tmp, Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	    CAL STOP VOLTAGE SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the cal stop voltage.		   										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetCalStopVoltage(ViSession vi, ViInt32 Snsr, 
										   ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CVSPV %s,%f\n", Temp[Snsr], Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	    CAL STOP VOLTAGE GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the cal stop voltage.   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalStopVoltage(ViSession vi, ViInt32 Snsr, 
										   ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[20], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CVSPV? %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f", 10, Tmp, 10, Tmp, Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL TABLE CLEAR FUNCTION        		  		*/
/*					          												*/
/*	Clears the cal table.   												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalTableClear(ViSession vi, ViInt32 Snsr, 
									   ViUInt8 TblNum)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CFUCT %s,%d\n", Temp[Snsr], TblNum)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL TABLE IDENTITY SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the cal table identity.  	 										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetCalTableIdentity(ViSession vi, ViInt32 Snsr, 
											 ViUInt8 Tbl, ViChar Ident[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CFUID %s,%d,%s\n", Temp[Snsr], Tbl, Ident)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL TABLE IDENTITY GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the cal table identity.   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalTableIdentity(ViSession vi, ViInt32 ISnsr, 
										 	 ViInt32 *OSnsr, ViUInt8 Tbl, 
										 	 ViChar Ident[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar TmpChr[3], Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "CFUID? %s,%d\n", Temp[ISnsr], Tbl));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "CFUID %s[w*t44q],%d,%s[w*t32q]", 2, TmpChr, &Tmp, 7, 
			 Ident));
	*OSnsr = SensorA;
	if(TmpChr[0]=='B')
		*OSnsr = SensorB;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL TABLE LOAD-READ SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the cal table.  	 												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetCalTableLoadRead(ViSession vi, ViInt32 Snsr, 
											 ViUInt8 Tbl, ViChar Ident[],
											 ViReal64 Freq[], ViReal64 Cal[],
											 ViUInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[700], Buf2[700], FrqBuf[4], CalBuf[2];
	ViInt32 i, BufSent, Unit;
	ViInt16 TmpI;

	Data32 LongData;
	Data16 TempData;
	
	sprintf(Buf,"CFULD ");
	
	checkErr(ML2430_GetCalDisplayUnits(vi, Snsr, &i, &Unit));
	
	Buf2[0] = 0x00;
	if(Tbl<1)
	    Tbl = 1;

	Buf[6] = Temp[Snsr][0];
	sprintf(Buf+7, ",%d", Tbl);
	while(strlen(Ident)<7)
		strcat(Ident," ");

	TempData.IVal = Sze;
	Buf2[0] = TempData.CVal[0];
	Buf2[1] = TempData.CVal[1];
	for(i=0; i<Sze; i++)
	{
		LongData.LVal = (ViInt32)(Freq[i]*.032768);
		FrqBuf[0] = LongData.CVal[0];
		FrqBuf[1] = LongData.CVal[1];
		FrqBuf[2] = LongData.CVal[2];
		FrqBuf[3] = LongData.CVal[3];
		if((Unit==Cal_Percent)||(Unit==Cal_PCT))
			TempData.IVal = (ViInt16)((20-10*log10(max(0.001,fabs(Cal[i]))))*1024);
		else
			TempData.IVal = (ViInt16)(Cal[i]*1024);
		CalBuf[0] = TempData.CVal[0];	   
		CalBuf[1] = TempData.CVal[1];	   
		memcpy(Buf2+2+i*6, FrqBuf, 4);
		memcpy(Buf2+6+i*6, CalBuf, 2);   
	}
	sprintf(Buf + strlen(Buf), ",%d,", ((Sze*6)+10));
	sprintf(Buf + strlen(Buf), "%s", Ident);
	memcpy(Buf + strlen(Buf) + 1, Buf2, ((Sze*6)+2));
	checkErr(viWrite(vi, Buf, (strlen(Buf)+3+Sze*6), &BufSent)); 
	
Error:
	return error;
}

/****************************************************************************/
/*			       	   CAL TABLE LOAD-READ GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the cal table.   												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetCalTableLoadRead(ViSession vi, ViInt32 Snsr, 
										 	 ViUInt8 Tbl, ViChar Ident[],
										 	 ViReal64 Freq[], ViReal64 Cal[],
										 	 ViInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"},*pComma=NULL;
	ViChar Buf[2000], Buf2[2000];
	ViInt32 Lngth=1900, BufSize = sizeof(Buf)-1, i = Tbl, Unit;
	ViReal64 TmpFlt;

	Data32 LongData;
	Data16 TempData;
	
	checkErr(viPrintf(vi, "CFURD %s,%d\n", Temp[Snsr], i)); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	Scan(Buf, "CFURD %d,", &Lngth);
	pComma = strstr(Buf, ",");
	if (pComma != NULL)
	  memcpy(Buf2, pComma, Lngth+1);
	else
	  memset(Buf2, '\0', Lngth+1);
	
	checkErr(ML2430_GetCalDisplayUnits(vi, Snsr, &i, &Unit));
	
	for(i=0; i<7; i++)
		Ident[i] = Buf2[i+1];
		
	TempData.CVal[0] = Buf2[9];
	TempData.CVal[1] = Buf2[10];
	Lngth = (ViInt16)TempData.IVal;
	if(Sze>Lngth)
		Sze = Lngth;
	
	for(i=0; i<Sze; i++)
	{
		LongData.CVal[0] = Buf2[11+i*6];
		LongData.CVal[1] = Buf2[12+i*6];
		LongData.CVal[2] = Buf2[13+i*6];
		LongData.CVal[3] = Buf2[14+i*6];
		Freq[i] = (ViReal64)LongData.LVal / .032768;
		TempData.CVal[0] = Buf2[15+i*6];
		TempData.CVal[1] = Buf2[16+i*6];
		if((Unit==Cal_Percent)||(Unit==Cal_PCT))
			Cal[i] = pow(10, (2-((ViReal64)TempData.IVal/10240)));
		else
			Cal[i] = (ViReal64)TempData.IVal / 1024;
	}
		
Error:
	return error;
}

/****************************************************************************/
/*			       	       CAL TABLE PRESET FUNCTION        		  		*/
/*					          												*/
/*	Sets the cal table preset.   											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalTablePreset(ViSession vi, ViInt32 Snsr, 
									   ViUInt8 TblNum)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CFUPT %s,%d\n", Temp[Snsr], TblNum)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL TABLE SAVE FUNCTION         		  		*/
/*					          												*/
/*	Saves the cal table.  		 											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalTableSave(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "CFUSAV\n")); 

Error:
	return error;
}

/****************************************************************************/
/*			       	       CAL TABLE SELECT FUNCTION        		  		*/
/*					          												*/
/*	Selects the cal table.   												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalTableSelect(ViSession vi, ViInt32 Snsr, 
									    ViUInt8 TblNum)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CFUSEL %s,%d\n", Temp[Snsr], TblNum)); 

Error:
	return error;
}

/****************************************************************************/
/*			       	       CAL TABLES COUNT FUNCTION        		  		*/
/*					          												*/
/*	Returns the number of cal tables.   									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalTablesCount(ViSession vi, ViInt32 Snsr, 
									    ViUInt8 *TblNum)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1, TmpInt;
	
	checkErr(viPrintf(vi, "CFUTBL %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &TmpInt));
	*TblNum = TmpInt;

Error:
	return error;
}

/****************************************************************************/
/*			       	        CAL TABLES USED FUNCTION        		  		*/
/*					          												*/
/*	Returns the number of cal tables used.   								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalTablesUsed(ViSession vi, ViInt32 Snsr, 
									   ViUInt8 *TblUsd)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "CFUUSE %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp)); 
	*TblUsd = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			       	      CAL VALIDITY CHECK FUNCTION        		  		*/
/*					          												*/
/*	Returns whether the cal is valid.		   								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalValidityCheck(ViSession vi, ViInt32 Snsr, 
									      ViUInt8 TblNum, ViInt32 *Validty)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CFUVLD %s,%d\n", Temp[Snsr], TblNum));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	*Validty = CalValid;
	if(strstr(Buf,"TRUE")==NULL)
		*Validty = CalInvalid;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	       CALIBRATE SENSOR FUNCTION        		  		*/
/*					          												*/
/*	Calibrates the sensor.   												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_CalibrateSensor(ViSession vi, ViInt32 Snsr)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "CAL %s\n", Temp[Snsr])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	  CHANNEL INPUT CONFIG SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the channel input configuration.									*/
/*																			*/
/****************************************************************************/
									//enum ChannelInputConfig      
ViStatus _VI_FUNC ML2430_SetChannelInputConfig(ViSession vi, ViInt32 Chan, 
											   ViInt32 Config)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[8][4] = {"OFF","A","B","V","A-B","B-A","A/B","B/A"};
	checkErr(viPrintf(vi, "CHCFG %d,%s\n", (Chan+1), Matrix[Config])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	  CHANNEL INPUT CONFIG GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the channel input configuration.  								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetChannelInputConfig(ViSession vi, ViInt32 IChan, 
										 	   ViInt32 *OChan, ViInt32 *Config)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1, TmpInt;
	checkErr(viPrintf(vi, "CHCFG? %d\n", (IChan+1))); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Config = Config_OFF;
	if(strstr(Buf, "OFF\n")==NULL)
		if(strstr(Buf, "A/B\n")==NULL)
			if(strstr(Buf, "B/A\n")==NULL)
				if(strstr(Buf, "V\n")==NULL)
					if(strstr(Buf, "A-B\n")==NULL)
						if(strstr(Buf, "B-A\n")==NULL)
							if(strstr(Buf, "A\n")==NULL)
								*Config = Config_B;
							else
								*Config = Config_A;
						else
							*Config = Config_B_A;
					else
						*Config = Config_A_B;
				else
					*Config = Config_V;
			else
				*Config = Config_BslashA;
		else
			*Config = Config_AslashB; 
	Scan(Buf, "CHCFG %d,", &TmpInt);
	*OChan = TmpInt-1;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	  CHANNEL RESOLUTION SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the channel resolution. 	 										*/
/*																			*/
/****************************************************************************/
									  //enum ChannelResolution    
ViStatus _VI_FUNC ML2430_SetChannelResolution(ViSession vi, ViInt32 Chan, 
											  ViInt32 Res)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "CHRES %d,%d\n", (Chan+1), (Res+1))); 

Error:
	return error;
}

/****************************************************************************/
/*			       	  CHANNEL RESOLUTION GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the channel resolution.   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetChannelResolution(ViSession vi, ViInt32 IChan, 
										 	  ViInt32 *OChan, ViInt32 *Res)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 TmpInt1, TmpInt2, Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "CHRES? %d\n", (IChan+1)));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "CHRES %d,%d", &TmpInt1, &TmpInt2));
	*OChan = TmpInt1-1;
	*Res = TmpInt2-1;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	       CHANNEL UNITS SET FUNCTION        		  		*/
/*					          												*/
/*	Sets the channel units.		 	 										*/
/*																			*/
/****************************************************************************/
									  //enum ChannelUnits    
ViStatus _VI_FUNC ML2430_SetChannelUnits(ViSession vi, ViInt32 Chan, 
										 ViInt32 Unit)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[4][5] = {"W","DBM","DBUV","DBMV"};
	checkErr(viPrintf(vi, "CHUNIT %d,%s\n", (Chan+1), Matrix[Unit])); 

Error:
	return error;
}

/****************************************************************************/
/*			       	       CHANNEL UNITS GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the channel units.		   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetChannelUnits(ViSession vi, ViInt32 IChan, 
										 ViInt32 *OChan, ViInt32 *Unit)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1, TmpInt;
	checkErr(viPrintf(vi, "CHUNIT? %d\n", (IChan+1))); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Unit = Chan_Watts;
	if(strstr(Buf, "W\n")==NULL)
		if(strstr(Buf, "DBM\n")==NULL)
			if(strstr(Buf, "DBUV\n")==NULL)
				*Unit = Chan_dBmV;
			else
				*Unit = Chan_dBuV;
		else
			*Unit = Chan_dB;
	Scan(Buf, "CHUNIT %d,", &TmpInt);
	*OChan = TmpInt-1;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	      CONDITIONS FOR SRQ FUNCTION        		  		*/
/*					          												*/
/*	Sets the conditions for the SRQ. 										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ConditionsForSRQ(ViSession vi, 
										  struct SRQConditions SRQCnd)
{
	ViStatus error = VI_SUCCESS;
	ViUInt8 Inpt = 0, Inpt2 = 0;
	
	if(SRQCnd.MsgAvail) Inpt2 = Inpt2 | ML2430_MAV;
	if((SRQCnd.OpComp)||(SRQCnd.ReqCont)||(SRQCnd.QuerErr)||(SRQCnd.DevDepErr)||
	   (SRQCnd.ExeErr)||(SRQCnd.ComErr)||(SRQCnd.UsrReq)||(SRQCnd.PowOn)) 
	    Inpt2 = Inpt2 | ML2430_ESB;

	if(SRQCnd.OpComp)    Inpt = Inpt | ML2430_OPC;
	if(SRQCnd.ReqCont)   Inpt = Inpt | ML2430_RQC;
	if(SRQCnd.QuerErr)   Inpt = Inpt | ML2430_QYE;
	if(SRQCnd.DevDepErr) Inpt = Inpt | ML2430_DDE;
	if(SRQCnd.ExeErr)    Inpt = Inpt | ML2430_EXE;
	if(SRQCnd.ComErr)    Inpt = Inpt | ML2430_CME;
	if(SRQCnd.UsrReq)    Inpt = Inpt | ML2430_URQ;
	if(SRQCnd.PowOn)     Inpt = Inpt | ML2430_PON;
	
	checkErr(ML2430_SetQueryEventStatusRegister(vi, Inpt));
	checkErr(ML2430_SetQuerySRQEnableRegister(vi, Inpt2));

Error:
	return error;
}

/****************************************************************************/
/*			       CONNECT POINTS ON PROFILE SET FUNCTION      		  		*/
/*					          												*/
/*	Sets on or off averaging between the file cursors.						*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetConnectPointsOnProfile(ViSession vi, ViInt32 Avgng)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "GRAVG %s\n", Matrix[Avgng])); 

Error:
	return error;
}

/****************************************************************************/
/*			       CONNECT POINTS ON PROFILE GET FUNCTION        	  		*/
/*					          												*/
/*	Returns whether averaging between the file cursors is on or off.		*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetConnectPointsOnProfile(ViSession vi, ViInt32 *Avgng)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "GRAVG?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*Avgng = Switch_On;
	if(strstr(ReadBuf, "ON")==NULL)
	    *Avgng = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*			       	           CONTINUE FUNCTION        	    	  		*/
/*					          												*/
/*	Allows continuation.			 										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Continue(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "CONT\n")); 

Error:
	return error;
}

/****************************************************************************/
/*			       	            CURSOR FUNCTION         	    	  		*/
/*					          												*/
/*	Selects the cursor.				 										*/
/*																			*/
/****************************************************************************/
						                //enum CursorSelect			      
ViStatus _VI_FUNC ML2430_Cursor(ViSession vi, ViInt32 Crsr, ViReal64 Val)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "CUR %d,%f\n", (Crsr+1), Val)); 

Error:
	return error;
}

/****************************************************************************/
/*			           DATA DISPLAY TIME SET FUNCTION        		  		*/
/*					          												*/
/*	Sets the data display time.		  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDataDisplayTime(ViSession vi, ViReal64 Tme, 
											ViInt32 Unit)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"MIN","HR"};
	checkErr(viPrintf(vi, "GRDDT %f,%s\n", Tme, Matrix[Unit])); 

Error:
	return error;
}

/****************************************************************************/
/*			           DATA DISPLAY TIME GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the data display time.	   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDataDisplayTime(ViSession vi, ViReal64 *Tme)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GRDDT?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "GRDDT %f", Tme)); 
	
Error:
	return error;
}

/****************************************************************************/
/*			       	    DISPLAY CONTRAST STEP FUNCTION          	  		*/
/*					          												*/
/*	Selects the display contrast step.				 										*/
/*																			*/
/****************************************************************************/
						                //enum CursorSelect			      
ViStatus _VI_FUNC ML2430_DisplayContrastStep(ViSession vi, ViInt32 Stp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"U","D"};
	checkErr(viPrintf(vi, "DCONT%s\n", Temp[Stp])); 

Error:
	return error;
}

/****************************************************************************/
/*			            DISPLAY CONTRAST SET FUNCTION        		  		*/
/*					          												*/
/*	Sets the display contrast.		  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDisplayContrast(ViSession vi, ViUInt8 Cntrst)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "DCONT %d\n", Cntrst)); 

Error:
	return error;
}

/****************************************************************************/
/*			            DISPLAY CONTRAST GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the display contrast.	   										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDisplayContrast(ViSession vi, ViUInt8 *Cntrst)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "DCONT?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "DCONT %d", &Tmp));
	*Cntrst = Tmp;
	
Error:
	return error;
}

/****************************************************************************/
/*			          DISPLAY GRAPH CURSOR SET FUNCTION        		  		*/
/*					          												*/
/*	Sets the cursor data on or off.	  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDisplayGraphCursor(ViSession vi, ViInt32 Crsr)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "GRDATA %s;\n", Matrix[Crsr])); 

Error:
	return error;
}

/****************************************************************************/
/*			          DISPLAY GRAPH CURSOR GET FUNCTION           	  		*/
/*					          												*/
/*	Returns whether the cursor data is on or off.							*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDisplayGraphCursor(ViSession vi, ViInt32 *Crsr)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "GRDATA?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*Crsr = Switch_On;
	if(strstr(ReadBuf, "ON")==NULL)
	    *Crsr = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*			             DISPLAY ON-OFF SET FUNCTION        		  		*/
/*					          												*/
/*	Sets the display on or off.	  											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDisplayOnOff(ViSession vi, ViInt32 Dsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "DISP %s;\n", Matrix[Dsp])); 

Error:
	return error;
}

/****************************************************************************/
/*			             DISPLAY ON-OFF GET FUNCTION            	  		*/
/*					          												*/
/*	Returns whether the display is on or off.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDisplayOnOff(ViSession vi, ViInt32 *Dsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "DISP?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*Dsp = Switch_On;
	if(strstr(ReadBuf, "ON")==NULL)
	    *Dsp = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*			          DISPLAY TRIGGER DELAY SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the display trigger delay.	  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDisplayTriggerDelay(ViSession vi, ViReal64 DspDly)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "DTRGD %.3f\n", DspDly)); 		  //%f was %g

Error:
	return error;
}

/****************************************************************************/
/*			          DISPLAY TRIGGER DELAY GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the display trigger delay.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDisplayTriggerDelay(ViSession vi, ViReal64 *DspDly)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "DTRGD?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "DTRGD %f", DspDly));  //%f was %d
	
Error:
	return error;
}

/****************************************************************************/
/*			                    DISPLAY FUNCTION       	        	  		*/
/*					          												*/
/*	Sets the display.	  													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Display(ViSession vi, ViInt32 Dsp, ViInt32 Cntrst,
								 ViInt32 PkDsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix1[2][4] = {"OFF","ON"};
	ViChar Matrix2[3][4] = {"A","B","A&B"};
	checkErr(viPrintf(vi, "DISP %s;DCONT %d;DPEAK %s\n", Matrix1[Dsp], 
					  Cntrst, Matrix2[PkDsp])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            DUTY CYCLE STATE SET FUNCTION       		  		*/
/*					          												*/
/*	Sets the duty cycle state.		  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDutyCycleState(ViSession vi, ViInt32 Snsr,
										   ViInt32 St)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "DUTYS %s,%s\n", Temp[Snsr], Matrix[St])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            DUTY CYCLE STATE GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the duty cycle state.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDutyCycleState(ViSession vi, ViInt32 ISnsr,
										   ViInt32 *OSnsr, ViInt32 *St)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "DUTYS? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*OSnsr = SensorA;
	if(strstr(ReadBuf, "A")==NULL)
	    *OSnsr = SensorB;
	*St = Switch_On;
	if(strstr(ReadBuf, "ON")==NULL)
	    *St = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*			               DUTY CYCLE SET FUNCTION          		  		*/
/*					          												*/
/*	Sets the duty cycle.			  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetDutyCycle(ViSession vi, ViInt32 Snsr, ViReal64 Cyc)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "DUTY %s,%f\n", Temp[Snsr], Cyc)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			               DUTY CYCLE GET FUNCTION              	  		*/
/*					          												*/
/*	Returns the duty cycle.													*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetDutyCycle(ViSession vi, ViInt32 ISnsr,
									  ViInt32 *OSnsr, ViReal64 *Cyc)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar ReadBuf[100], TmpStr[20];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	
	checkErr(viPrintf(vi, "DUTY? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	checkErr(Scan(ReadBuf, "%s[w*t32q] %s[w*t44q],%f", 19, TmpStr, 3, TmpStr, 
				  Cyc));
	*OSnsr = SensorA;
	if(strstr(TmpStr, "A")==NULL)
	    *OSnsr = SensorB;
	
Error:
	return error;
}

/****************************************************************************/
/*			                  EMULATION FUNCTION       	        	  		*/
/*					          												*/
/*	Sets the emulation.	  													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Emulation(ViSession vi, ViInt32 Emul)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[5][7] = {"ML24XX","HP436A","HP437B","HP438A","ML4803"};
	checkErr(viPrintf(vi, "EMUL %s\n", Matrix[Emul])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            ENTRY ERROR BEEP SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the entry error beep on or off.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetEntryErrorBeep(ViSession vi, ViInt32 Beep)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "ENTERR %s;\n", Matrix[Beep])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			             ENTRY ERROR BEEP GET FUNCTION             	  		*/
/*					          												*/
/*	Returns whether the entry error beep is on or off.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetEntryErrorBeep(ViSession vi, ViInt32 *Beep)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "ENTERR?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*Beep = Switch_On;
	if(strstr(ReadBuf, "ON")==NULL)
	    *Beep = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			                ERROR LISTING FUNCTION          		  		*/
/*					          												*/
/*	Lists all received errors.		  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ErrorListing(ViSession vi, 
									  struct ErrorListSensor *SnsrA,
									  struct ErrorListSensor *SnsrB, 
									  struct ErrorListChannel *Chan1,
									  struct ErrorListChannel *Chan2,
									  ViInt32 *PrntrErr, ViChar ExeErr[8],
									  ViChar ComErr[8])
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100], TmpBuf[100], TmpBuf2[100], TmpChr[2];
	ViInt32 BufSize = sizeof(ReadBuf)-1, TmpBufSize = 100, ComLngth, ExeLngth;
	checkErr(viPrintf(vi, "ERRLST\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize));
	ReadBuf[BufSize]=0x00;

	TmpChr[1] = 0x00;
	TmpChr[0] = ReadBuf[0];
	if ( SnsrA ) SnsrA->ZeroState       = atol(TmpChr);
	TmpChr[0] = ReadBuf[1];
	if ( SnsrB ) SnsrB->ZeroState       = atol(TmpChr);
	TmpChr[0] = ReadBuf[2];
	if ( SnsrA ) SnsrA->CALState        = atol(TmpChr);
	TmpChr[0] = ReadBuf[3];
	if ( SnsrB ) SnsrB->CALState        = atol(TmpChr);
	TmpChr[0] = ReadBuf[4];
	if ( SnsrA ) SnsrA->RangeHold       = atol(TmpChr);
	TmpChr[0] = ReadBuf[5];
	if ( SnsrB ) SnsrB->RangeHold       = atol(TmpChr);
	TmpChr[0] = ReadBuf[6];
	if ( Chan1 ) Chan1->OutOfRange      = atol(TmpChr);
	TmpChr[0] = ReadBuf[7];
	if ( Chan2 ) Chan2->OutOfRange      = atol(TmpChr);
	TmpChr[0] = ReadBuf[8];
	if ( Chan1 ) Chan1->IllegalLogOp    = atol(TmpChr);
	TmpChr[0] = ReadBuf[9];
	if ( Chan2 ) Chan2->IllegalLogOp    = atol(TmpChr);
	TmpChr[0] = ReadBuf[10];
	if ( PrntrErr ) *PrntrErr			= atol(TmpChr);
	TmpChr[0] = ReadBuf[11];
	if ( SnsrA ) SnsrA->FittedUsedState = atol(TmpChr);
	TmpChr[0] = ReadBuf[12];
	if ( SnsrB ) SnsrB->FittedUsedState = atol(TmpChr);
	TmpChr[0] = ReadBuf[13];
	if ( Chan1 ) Chan1->LimitsState	   = atol(TmpChr);
	TmpChr[0] = ReadBuf[14];
	if ( Chan2 ) Chan2->LimitsState	   = atol(TmpChr);

	if(strstr(ReadBuf, "!!!")==NULL)
	{
		strncpy(TmpBuf, (strstr(ReadBuf, "!")+1), TmpBufSize);
		ComLngth = strstr(TmpBuf, "!") - TmpBuf;
		strncpy(ComErr, (strstr(ReadBuf, "!")+1), ComLngth);
		ComErr[ComLngth] = 0x00;
		if(strstr(TmpBuf, "!!")==NULL)
		{
			strncpy(TmpBuf2, (strstr(TmpBuf, "!")+1), TmpBufSize);
			ExeLngth = strstr(TmpBuf2, "!") - TmpBuf2;
			strncpy(ExeErr, (strstr(TmpBuf, "!")+1), ExeLngth);
			ExeErr[ExeLngth] = 0x00;
		}
		else
			ExeErr = 0x00;
	}
	else
	{
		ExeErr = 0x00;
		ComErr = 0x00;
	}

Error:
	return error;
}

/****************************************************************************/
/*			            FAIL BEEP ON-OFF SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the fail beep on or off.	  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetFailBeepOnOff(ViSession vi, ViInt32 Chan, 
										  ViInt32 Beep)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "FBEEP %d,%s\n", (Chan+1), Matrix[Beep])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            FAIL BEEP ON-OFF GET FUNCTION             	  		*/
/*					          												*/
/*	Returns whether the fail beep is on or off.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetFailBeepOnOff(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Beep)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[5], Buf[100];
	ViInt32 TmpInt, Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "FBEEP? %d\n", (IChan+1)));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "FBEEP %d,%s", &TmpInt, TmpStr)); 
	*OChan = TmpInt-1;
	*Beep = Switch_On;
	if(strstr(TmpStr,"ON")==NULL)
		*Beep = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			          FAIL INDICATOR HOLD SET FUNCTION         		  		*/
/*					          												*/
/*	Sets the fail indicator hold.	  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetFailIndicatorHold(ViSession vi, ViInt32 Chan, 
										  	  ViInt32 Fail)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "FHOLD %d,%s\n", (Chan+1), Matrix[Fail])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          FAIL INDICATOR HOLD GET FUNCTION             	  		*/
/*					          												*/
/*	Returns the fail indicator hold.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetFailIndicatorHold(ViSession vi, ViInt32 IChan, 
										  	  ViInt32 *OChan, ViInt32 *Fail)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[5], Buf[100];
	ViInt32 TmpInt, Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "FHOLD? %d\n", (IChan+1))); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "FHOLD %d,%s", &TmpInt, TmpStr)); 
	*OChan = TmpInt-1;
	*Fail = Switch_Off;
	if(strstr(TmpStr,"OFF")==NULL)
		*Fail = Switch_On;

Error:
	return error;
}

/****************************************************************************/
/*			                    FAST FUNCTION               		  		*/
/*					          												*/
/*	Sets fast on or off.			  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Fast(ViSession vi, ViInt32 Fast)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "FAST %s\n", Matrix[Fast])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			       FREQUENCY - OFFSET DISPLAY SET FUNCTION     		  		*/
/*					          												*/
/*	Sets the frequency offset display on or off.							*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetFrequencyOffsetDisplay(ViSession vi, ViInt32 Dsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "FROFF %s\n", Matrix[Dsp])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			       FREQUENCY - OFFSET DISPLAY GET FUNCTION         	  		*/
/*					          												*/
/*	Returns whether the frequency offset display is on or off.				*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetFrequencyOffsetDisplay(ViSession vi, ViInt32 *Dsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar ReadBuf[100];
	ViUInt32 BufSize = sizeof(ReadBuf)-1;
	checkErr(viPrintf(vi, "FROFF?\n")); 
	checkErr(viRead(vi, ReadBuf, BufSize, &BufSize)); 
	ReadBuf[BufSize]=0x00;

	*Dsp = Switch_On;
	if(strstr(ReadBuf, "ON")==NULL)
		*Dsp = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			               GET CONFIGURATION FUNCTION          		  		*/
/*					          												*/
/*	Sets the GT trigger configuration. 										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GETConfiguration(ViSession vi, ViInt32 Get, 
										  ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"3","0"};
	checkErr(viPrintf(vi, "GT%d;TR%s\n", Get, Temp[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			                GPIB ADDRESS SET FUNCTION        		  		*/
/*					          												*/
/*	Sets the GPIB Address.	  												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetGPIBAddress(ViSession vi, ViInt32 Addr)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "ADDR %d\n", Addr)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			                GPIB ADDRESS GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the GPIB address.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetGPIBAddress(ViSession vi, ViInt32 *Addr)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "ADDR?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "ADDR %d", Addr)); 

Error:
	return error;
}

/****************************************************************************/
/*			             GPIB GET TRIGGER TYPE FUNCTION       		  		*/
/*					          												*/
/*	Sets the GET trigger type.		  										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GPIBGETTriggerType(ViSession vi, ViInt32 Get)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GT%d\n", Get)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            GPIB RESPONSE BUFFERING FUNCTION       		  		*/
/*					          												*/
/*	Sets the GPIB response buffering on or off.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GPIBResponseBuffering(ViSession vi, ViInt32 RspB)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "BUFF %s\n", Matrix[RspB])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			               GPIB TRIGGER MODE FUNCTION       		  		*/
/*					          												*/
/*	Sets the GPIB trigger mode.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GPIBTriggerMode(ViSession vi, ViInt32 Mode, 
										 ViInt32 Chan)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[4][4] = {"TR0","TR1","TR2","TR3"};
	if((Mode==1)||(Mode==2))
		checkErr(viPrintf(vi, "%s%d\n", Matrix[Mode], (Chan+1))); 		  
	else
		checkErr(viPrintf(vi, "%s\n", Matrix[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           GRAPH DATA INFORMATION FUNCTION       		  		*/
/*					          												*/
/*	Returns the graph data information.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GraphDataInformation(ViSession vi, ViUInt32 *ChanNum,
											  ViReal64 *Crsr1, ViReal64 *Crsr2,
											  ViReal64 *DltaPow, 
											  ViReal64 *DltaTme, ViReal64 *Avg, 
											  ViReal64 *MinVal, 
											  ViReal64 *MaxVal)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf1[80], Buf2[40];
	ViInt32 BufSize1 = sizeof(Buf1)-1, BufSize2 = sizeof(Buf2)-1, Tmp, Avgng;
	
	checkErr(viPrintf(vi, "GRDRQ\n")); 		  
	checkErr(viRead(vi, Buf1, BufSize1, &BufSize1));
	Buf1[BufSize1]=0x00;

	checkErr(ML2430_GetConnectPointsOnProfile(vi, &Avgng));
	if(Avgng==Switch_On)
		Scan(Buf1, "GRDRQ %d,%f,%f,%f,%f,%f", &Tmp, Crsr1, Crsr2, DltaPow,
			 DltaTme, Avg);
	else
	{
		Scan(Buf1, "GRDRQ %d,%f,%f,%f,%f", &Tmp, Crsr1, Crsr2, DltaPow,
			 DltaTme);
		*Avg = 0.00;		 
	}
	*ChanNum = Tmp-1;
		 
	checkErr(viPrintf(vi, "GMNMX %d\n", Tmp)); 		  
	checkErr(viRead(vi, Buf2, BufSize2, &BufSize2));
	Buf2[BufSize2]=0x00;

	Scan(Buf2, "%f,%f,", MinVal, MaxVal);
	
Error:
	return error;
}

/****************************************************************************/
/*			       GRAPH PRETRIGGER PERCENTAGE SET FUNCTION     	  		*/
/*					          												*/
/*	Sets the graph pretrigger percentage.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetGraphPretriggerPercentage(ViSession vi, ViUInt8 Pct)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GRPTP %d\n", Pct)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			       GRAPH PRETRIGGER PERCENTAGE GET FUNCTION        	  		*/
/*					          												*/
/*	Returns the graph pretrigger percentage.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetGraphPretriggerPercentage(ViSession vi, 
													  ViUInt8 *Pct)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "GRPTP?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "GRPTP %d", &Tmp));
	*Pct = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			                    HOLD SET FUNCTION                	  		*/
/*					          												*/
/*	Sets the hold on or off.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetHold(ViSession vi, ViInt32 Hld)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "HOLD %s\n", Matrix[Hld])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			                    HOLD GET FUNCTION               	  		*/
/*					          												*/
/*	Returns whether the hold is on or off.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetHold(ViSession vi, ViInt32 *Hld)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "HOLD?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize)); 
	Buf[BufSize]=0x00;

	*Hld = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Hld = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			                IDENTIFICATION FUNCTION                  	  	*/
/*					          												*/
/*	Returns the instrument identification.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Identification(ViSession vi, ViChar Ident[32])
{
	ViStatus error = VI_SUCCESS;
	ViInt32 Bufsize = 31;
	checkErr(viPrintf(vi, "*IDN?\n"));
	checkErr(viRead(vi, Ident, Bufsize, &Bufsize));
	Ident[Bufsize]=0x00;

Error:
	return error;
}

/****************************************************************************/
/*			                   SELF TEST FUNCTION                    	  	*/
/*					          												*/
/*	Returns 1 if the self test passed, 0 if failed.							*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_TestSelf(ViSession vi, ViInt32 *Sttus)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "*TST?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	*Sttus = 1;
	if(strstr(Buf,"PASSED")==NULL)
		*Sttus = 0;

Error:
	return error;
}

/****************************************************************************/
/*			            OPERATION COMPLETE SET FUNCTION                	  	*/
/*					          												*/
/*	Sets the operation complete bit in the Event Status Register.			*/
/*								        									*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetOperationComplete(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*OPC\n"));

Error:
	return error;
}

/****************************************************************************/
/*			            OPERATION COMPLETE GET FUNCTION                	  	*/
/*					          												*/
/*	Returns a '1' when all operations are complete.             			*/
/*								        									*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetOperationComplete(ViSession vi, ViInt32 *Sttus)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "*OPC?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	*Sttus = 1;
	if(strstr(Buf, "1")==NULL)
		*Sttus = 0;

Error:
	return error;
}

/****************************************************************************/
/*			              CLEAR STATUS BYTES FUNCTION                  	  	*/
/*					          												*/
/*	Clears the status bytes.            									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ClearStatusBytes(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*CLS\n"));

Error:
	return error;
}

/****************************************************************************/
/*			         EVENT STATUS BYTE ENABLE SET FUNCTION             	  	*/
/*					          												*/
/*	Enables event status.               									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetEventStatusByteEnable(ViSession vi, ViUInt8 Msk)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*ESE %d\n", Msk));

Error:
	return error;
}

/****************************************************************************/
/*			         EVENT STATUS BYTE ENABLE GET FUNCTION             	  	*/
/*					          												*/
/*	Returns the event status byte.         									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetEventStatusByteEnable(ViSession vi, ViUInt8 *Byt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "*ESE?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp));
	*Byt = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			              EVENT STATUS REGISTER FUNCTION             	  	*/
/*					          												*/
/*	Returns the event status register byte.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_EventStatusRegister(ViSession vi, ViUInt8 *Byt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "*ESR?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp));
	*Byt = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			      SERVICE REQUEST ENABLE REGISTER SET FUNCTION         	  	*/
/*					          												*/
/*	Sets the service request enable register byte.		    				*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetServiceRequestRegister(ViSession vi, ViUInt8 Msk)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*SRE %d\n", Msk));

Error:
	return error;
}

/****************************************************************************/
/*			      SERVICE REQUEST ENABLE REGISTER GET FUNCTION         	  	*/
/*					          												*/
/*	Returns the service request enable register byte.		    			*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetServiceRequestRegister(ViSession vi, ViUInt8 *Byt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "*SRE?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp));
	*Byt = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			             STATUS BYTE REGISTER FUNCTION              	  	*/
/*					          												*/
/*	Returns the status byte.                        		    			*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_StatusByteRegister(ViSession vi, ViUInt8 *Byt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[25];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "*STB?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp));
	*Byt = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			            GROUP EXECUTE TRIGGER FUNCTION              	  	*/
/*					          												*/
/*	Returns a trigger and output for each channel that is on.    			*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GroupExecuteTrigger(ViSession vi, ViChar Buf[], 
											 ViInt32 Bufsize)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*TRG\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;


Error:
	return error;
}

/****************************************************************************/
/*			            SAVE INSTRUMENT SETUP FUNCTION                	  	*/
/*					          												*/
/*	Saves the instrument setup.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SaveInstrumentSetup(ViSession vi, ViInt32 Num)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*SAV %d\n", Num)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           RECALL INSTRUMENT SETUP FUNCTION                	  	*/
/*					          												*/
/*	Recalls the instrument setup.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_RecallInstrumentSetup(ViSession vi, ViInt32 Num)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*RCL %d\n", Num)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			               INITIALIZE MODEM FUNCTION                	  	*/
/*					          												*/
/*	Initializes the modem.      											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_InitializeModem(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "MODINIT\n")); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          KEY CLICK SOUND ON-OFF SET FUNCTION          	  		*/
/*					          												*/
/*	Sets the key click sound on or off.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetKeyClickSoundOnOff(ViSession vi, ViInt32 Snd)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "KEYCK %s;\n", Matrix[Snd])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          KEY CLICK SOUND ON-OFF GET FUNCTION          	  		*/
/*					          												*/
/*	Returns whether the key click sound is on or off.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetKeyClickSoundOnOff(ViSession vi, ViInt32 *Snd)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "KEYCK?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize)); 
	Buf[BufSize]=0x00;

	*Snd = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Snd = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			              LIMITS ON-OFF SET FUNCTION            	  		*/
/*					          												*/
/*	Sets the limits on or off.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetLimitsOnOff(ViSession vi, ViInt32 Chan, 
										ViInt32 HLim, ViInt32 LLim)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "HLIMS %d,%s;LLIMS %d,%s\n", (Chan+1), Matrix[HLim],
					  (Chan+1), Matrix[LLim])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			              LIMITS ON-OFF GET FUNCTION            	  		*/
/*					          												*/
/*	Returns whether the limits are on or off.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetLimitsOnOff(ViSession vi, ViInt32 Chan, 
										ViInt32 *HLim, ViInt32 *LLim)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf1[200], Buf2[100];
	ViUInt32 BufSize1 = sizeof(Buf1)-1, BufSize2 = sizeof(Buf2)-1;
	checkErr(viPrintf(vi, "HLIMS? %d\n", (Chan+1))); 
	checkErr(viRead(vi, Buf1, BufSize1, &BufSize1)); 
	Buf1[BufSize1]=0x00;

	Buf1[BufSize1]=0x00;
	*HLim = Switch_On;
	if(strstr(Buf1, "ON")==NULL)
		*HLim = Switch_Off;
		
	checkErr(viPrintf(vi, "LLIMS? %d\n", (Chan+1))); 
	checkErr(viRead(vi, Buf2, BufSize2, &BufSize2)); 
	Buf2[BufSize2]=0x00;

	*LLim = Switch_On;
	if(strstr(Buf2, "ON")==NULL)
		*LLim = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			              LIMITS SETTING SET FUNCTION            	  		*/
/*					          												*/
/*	Sets the limits.														*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetLimitsSetting(ViSession vi, ViInt32 Chan, 
										  ViReal64 HLim, ViReal64 LLim)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "HLIM %d,%f;LLIM %d,%f\n", (Chan+1), HLim,
					  (Chan+1), LLim)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			              LIMITS SETTING GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the limits.														*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetLimitsSetting(ViSession vi, ViInt32 Chan, 
										  ViReal64 *HLim, ViReal64 *LLim)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	ViInt32 BytesRead;
	
	checkErr(viPrintf(vi, "HLIM? %d\n", (Chan+1)));
	checkErr(viRead(vi, Buf, Bufsize, &BytesRead));
	Buf[BytesRead]=0x00;
	checkErr(Scan(Buf, "HLIM %d,%f", &Tmp, HLim));
	
	checkErr(viPrintf(vi, "LLIM? %d\n", (Chan+1))); 
	checkErr(viRead(vi, Buf, Bufsize, &BytesRead));
	Buf[BytesRead]=0x00;
	checkErr(Scan(Buf, "LLIM %d,%f", &Tmp, LLim)); 

Error:
	return error;
}

/****************************************************************************/
/*			             LINK GRAPH CURSORS SET FUNCTION           	  		*/
/*					          												*/
/*	Sets the link graph cursors on or off.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetLinkGraphCursors(ViSession vi, ViInt32 Lnk)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "CURLK %s;\n", Matrix[Lnk])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			             LINK GRAPH CURSORS GET FUNCTION           	  		*/
/*					          												*/
/*	Returns whether the link graph cursors are on or off.					*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetLinkGraphCursors(ViSession vi, ViInt32 *Lnk)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "CURLK?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize)); 
	Buf[BufSize]=0x00;

	*Lnk = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Lnk = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			             MIN MAX TRACKING SET FUNCTION           	  		*/
/*					          												*/
/*	Sets the min max tracking on or off.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetMinMaxTracking(ViSession vi, ViInt32 Chan, 
										   ViInt32 Mnmx)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "MNMXS %d,%s\n", (Chan+1), Matrix[Mnmx])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			             MIN MAX TRACKING GET FUNCTION           	  		*/
/*					          												*/
/*	Returns whether the min max tracking is on or off.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetMinMaxTracking(ViSession vi, ViInt32 IChan, 
										   ViInt32 *OChan, ViInt32 *Mnmx)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[8], Buf[100];
	ViInt32 TmpInt, Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "MNMXS? %d\n", (IChan+1)));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "MNMXS %d,%s", &TmpInt, TmpStr)); 
	*OChan = TmpInt-1;
	*Mnmx = Switch_On;
	if(strstr(TmpStr,"ON")==NULL)
		*Mnmx = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			            MIN MAX TRACKING RESET FUNCTION           	  		*/
/*					          												*/
/*	Sets the min max tracking on or off.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_MinMaxTrackingReset(ViSession vi, ViInt32 Chan)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "MMRST %d\n", (Chan+1))); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          MODEM REDIAL DELAY TIME SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the modem redial delay time.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetModemRedialDelayTime(ViSession vi, ViUInt8 Dly)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "MODDEL %d\n", Dly)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          MODEM REDIAL DELAY TIME GET FUNCTION         	  		*/
/*					          												*/
/*	Returns the modem redial delay time.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetModemRedialDelayTime(ViSession vi, ViUInt8 *Dly)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "MODDEL?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "MODDEL %d", &Tmp));
	*Dly = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			            OFFSET FIXED VALUE SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the offset fixed value.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetOffsetFixedValue(ViSession vi, ViInt32 Snsr,
											 ViReal64 OffSt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "OFFFIX %s,%f\n", Temp[Snsr], OffSt)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            OFFSET FIXED VALUE GET FUNCTION         	  		*/
/*					          												*/
/*	Returns the offset fixed value.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetOffsetFixedValue(ViSession vi, ViInt32 ISnsr,
											 ViInt32 *OSnsr, ViReal64 *OffSt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"}, Buf[100], TmpChr[10];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "OFFFIX? %s\n", Temp[ISnsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f", 9, TmpChr, 9, TmpChr, OffSt));
	*OSnsr = SensorA;
	if(TmpChr[0]=='B')
		*OSnsr = SensorB;

Error:
	return error;
}

/****************************************************************************/
/*			              OFFSET TABLE CLEAR FUNCTION           	  		*/
/*					          												*/
/*	Clears the offset table.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OffsetTableClear(ViSession vi, ViUInt8 Tbl)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "OFFCLR %d\n", Tbl)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			       OFFSET TABLE OUTPUT-UPDATE SET FUNCTION         	  		*/
/*					          												*/
/*	Updates an offset table.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetOffsetTableOutputUpdate(ViSession vi, 
													ViUInt8 TblNum,
											 		ViReal64 Freq[],
											 		ViReal64 OffSt[], 
											 		ViInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[1980], NumChr1[4], NumChr2[5], BufO[2000];
	ViInt32 i, NewSze;
	float Fl = 10000;
	char *pFl = (char*)&Fl;
	NumChr2[4] = 0x00;
	
	for(i=0; i<Sze; i++)
	{
		error = ML2430_FloatToFourBytes(Freq[i], NumChr2);
		//NumChr2[0] = NumChr1[1];
		//NumChr2[1] = NumChr1[0];
		//NumChr2[2] = NumChr1[3];
		//NumChr2[3] = NumChr1[2];
		memcpy(Buf+i*8,NumChr2,4);
		error = ML2430_FloatToFourBytes(OffSt[i], NumChr2);
		//NumChr2[0] = NumChr1[1];
		//NumChr2[1] = NumChr1[0];
		//NumChr2[2] = NumChr1[3];
		//NumChr2[3] = NumChr1[2];
		memcpy(Buf+i*8+4,NumChr2,4);
	}
	sprintf(BufO,"OFFTBU %d,%d,", TblNum, Sze*8);
	NewSze = strlen(BufO);
	memcpy(BufO+NewSze,Buf,Sze*8);
	checkErr(viWrite(vi, BufO, NewSze+Sze*8, &NewSze));
	
Error:
	return error;
}

/****************************************************************************/
/*			       OFFSET TABLE OUTPUT-UPDATE GET FUNCTION         	  		*/
/*					          												*/
/*	Outputs an offset table.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetOffsetTableOutputUpdate(ViSession vi, 
													ViUInt8 TblNum,
											 		ViReal64 Freq[],
											 		ViReal64 OffSt[], 
											 		ViInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2000], Buf2[2000], TmpStr1[4], TmpStr2[4], TmpBuf[10], TmpBuf2[5];
	ViChar *pComma=NULL;
	ViInt32 BufSize = sizeof(Buf)-1, Tmp, i;
	ViReal64 TmpFlt;
	
	checkErr(viPrintf(vi, "OFFTBR %d\n", TblNum));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	memcpy(TmpBuf, Buf, 9);
	Tmp = TmpBuf[8] - '0';
	memcpy(TmpBuf2, Buf+9, Tmp);
	TmpBuf2[4] = 0x00;
	Tmp = atoi(TmpBuf2);
	pComma=strstr(Buf, ",");
	if (pComma!=NULL)
	  memcpy(Buf2, pComma+1, Tmp);
	else
	  memset(Buf2, '\0', Tmp);
	Sze *= 8;
	if(Sze>Tmp)
		Sze = Tmp;
		
	for(i=0; i<Sze; i+=8)
	{
		TmpStr1[0] = Buf2[i];
		TmpStr1[1] = Buf2[i+1];
		TmpStr1[2] = Buf2[i+2];
		TmpStr1[3] = Buf2[i+3];
		error = ML2430_FourBytesToFloat(TmpStr1, &TmpFlt);
		Freq[(i/8)] = TmpFlt;
		TmpStr2[0] = Buf2[i+4];
		TmpStr2[1] = Buf2[i+5];
		TmpStr2[2] = Buf2[i+6];
		TmpStr2[3] = Buf2[i+7];
		error = ML2430_FourBytesToFloat(TmpStr2, &TmpFlt);
		OffSt[(i/8)] = TmpFlt;
	}

Error:
	return error;
}

/****************************************************************************/
/*			              OFFSET TABLE SET FUNCTION             	  		*/
/*					          												*/
/*	Sets the offset table.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetOffsetTable(ViSession vi, ViInt32 Snsr, ViUInt8 Tbl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "OFFTBL %s,%d\n", Temp[Snsr], Tbl)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			              OFFSET TABLE GET FUNCTION             	  		*/
/*					          												*/
/*	Returns the offset table.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetOffsetTable(ViSession vi, ViInt32 ISnsr, 
										ViInt32 *OSnsr, ViUInt8 *Tbl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"}, Buf[100], TmpChr[20]; 
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "OFFTBL? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%d.", 19, TmpChr, 2, TmpChr, &Tmp));
	*Tbl = Tmp;
	*OSnsr = SensorA;
	if(TmpChr[0]=='B')
		*OSnsr = SensorB;

Error:
	return error;
}

/****************************************************************************/
/*			             OPERATION MODE SET FUNCTION             	  		*/
/*					          												*/
/*	Sets the operation mode.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetOperationMode(ViSession vi, ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[4][8] = {"DIGIT","PROFILE","PWRTIM","SRCSWP"};
	checkErr(viPrintf(vi, "OPMD %s\n", Matrix[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			             OPERATION MODE GET FUNCTION             	  		*/
/*					          												*/
/*	Returns the operation mode.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetOperationMode(ViSession vi, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "OPMD?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Mode = Mode_Digit;
	if(strstr(Buf, "DIGIT\n")==NULL)
		if(strstr(Buf, "PWRTIM\n")==NULL)
			if(strstr(Buf, "PROFILE\n")==NULL)
				*Mode = Mode_Srcswp;
			else
				*Mode = Mode_Profile;
		else
			*Mode = Mode_Pwrtim;
	 
Error:
	return error;
}

/****************************************************************************/
/*			           OUTPUT GRAPH BINARY DATA FUNCTION           	  		*/
/*					          												*/
/*	Outputs graph binary data.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputGraphBinaryData(ViSession vi, ViReal64 Dta[], 
											   ViInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2000], Temp[10];
	ViInt32 Bufsize = sizeof(Buf)-1, LLng, Lngth, i;
	Data32 LongData;

	checkErr(viPrintf(vi, "OGBD\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	memcpy(Temp, Buf, 7);
	Scan(Temp, "OGBD #%d", &LLng);
	memcpy(Temp, Buf+7, LLng);
	Temp[LLng] = 0x00;
	Lngth = atol(Temp);
	if(Sze>(Lngth/4))
		Sze=(Lngth/4);
	
	for(i=0; i<Sze; i++)
	{
		LongData.CVal[0] = Buf[(7+LLng+i*4)];
		LongData.CVal[1] = Buf[(8+LLng+i*4)];
		LongData.CVal[2] = Buf[(9+LLng+i*4)];
		LongData.CVal[3] = Buf[(10+LLng+i*4)];
		Dta[i] = (ViReal64)(LongData.LVal / 1024);
	}

Error:
	return error;
}

/****************************************************************************/
/*			               OUTPUT GRAPH DATA FUNCTION           	  		*/
/*					          												*/
/*	Returns output graph data.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputGraphData(ViSession vi, ViReal64 Dta[200])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2048];
	ViInt32 i, Bufsize = sizeof(Buf)-1, NumPts;
	ViReal64 Tmp;
	checkErr(viPrintf(vi, "OGD\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "OGD %d,", &NumPts));
	for(i=0; i<NumPts; i++)
	{
		Scan(Buf+7+(i*10), ",%f", &Tmp);
		Dta[i] = Tmp;
	}
	
Error:
	return error;
}

/****************************************************************************/
/*			         OUTPUT MAX GRAPH BINARY DATA FUNCTION         	  		*/
/*					          												*/
/*	Outputs max graph binary data.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputMaxGraphBinaryData(ViSession vi, ViReal64 Dta[],
												  ViInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2000], Temp[10];
	ViInt32 Bufsize = sizeof(Buf)-1, LLng, Lngth, i;
	Data32 LongData;

	checkErr(viPrintf(vi, "MXGDB\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	memcpy(Temp, Buf, 8);
	Scan(Temp, "MXGDB #%d", &LLng);
	memcpy(Temp, Buf+8, LLng);
	Temp[LLng] = 0x00;
	Lngth = atol(Temp);
	if(Sze>(Lngth/4))
		Sze=(Lngth/4);
	
	for(i=0; i<Sze; i++)
	{
		LongData.CVal[0] = Buf[(8+LLng+i*4)];
		LongData.CVal[1] = Buf[(9+LLng+i*4)];
		LongData.CVal[2] = Buf[(10+LLng+i*4)];
		LongData.CVal[3] = Buf[(11+LLng+i*4)];
		Dta[i] = (ViReal64)(LongData.LVal / 1024);
	}

Error:
	return error;
}

/****************************************************************************/
/*			             OUTPUT MAX GRAPH DATA FUNCTION         	  		*/
/*					          												*/
/*	Outputs max graph data.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputMaxGraphData(ViSession vi, ViReal64 Dta[200])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2048];
	ViInt32 i, Bufsize = sizeof(Buf)-1, NumPts;
	ViReal64 Tmp;
	checkErr(viPrintf(vi, "MXGD\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "MXGD %d,", &NumPts));
	for(i=0; i<NumPts; i++)
	{
		Scan(Buf+8+(i*10), ",%f", &Tmp);
		Dta[i] = Tmp;
	}

Error:
	return error;
}

/****************************************************************************/
/*			         OUTPUT MIN GRAPH BINARY DATA FUNCTION         	  		*/
/*					          												*/
/*	Outputs min graph binary data.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputMinGraphBinaryData(ViSession vi, ViReal64 Dta[],
												  ViInt32 Sze)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2000], Temp[10];
	ViInt32 Bufsize = sizeof(Buf)-1, LLng, Lngth, i;
	Data32 LongData;
	
	checkErr(viPrintf(vi, "MNGDB\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	memcpy(Temp, Buf, 8);
	Scan(Temp, "MNGBD #%d", &LLng);
	memcpy(Temp, Buf+8, LLng);
	Temp[LLng] = 0x00;
	Lngth = atol(Temp);
	if(Sze>(Lngth/4))
		Sze=(Lngth/4);
	
	for(i=0; i<Sze; i++)
	{
		LongData.CVal[0] = Buf[(8+LLng+i*4)];
		LongData.CVal[1] = Buf[(9+LLng+i*4)];
		LongData.CVal[2] = Buf[(10+LLng+i*4)];
		LongData.CVal[3] = Buf[(11+LLng+i*4)];
		Dta[i] = (ViReal64)(LongData.LVal / 1024);
	}

Error:
	return error;
}

/****************************************************************************/
/*			             OUTPUT MIN GRAPH DATA FUNCTION         	  		*/
/*					          												*/
/*	Outputs min graph data.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputMinGraphData(ViSession vi, ViReal64 Dta[200])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2048];
	ViInt32 i, Bufsize = sizeof(Buf)-1, NumPts;
	ViReal64 Tmp;
	checkErr(viPrintf(vi, "MNGD\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "MNGD %d,", &NumPts));
	for(i=0; i<NumPts; i++)
	{
		Scan(Buf+8+(i*10), ",%f", &Tmp);
		Dta[i] = Tmp;
	}

Error:
	return error;
}

/****************************************************************************/
/*			       OUTPUT NUMBER OF CHANNEL READINGS FUNCTION      	  		*/
/*					          												*/
/*	Outputs a number of channel readings.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputNumberOfChannelReadings(ViSession vi, 
													   ViInt32 Chan,
													   ViUInt16 NumRds,
													   ViReal64 Rslt[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100000];
	ViInt32 BufSize = sizeof(Buf)-1, i;
	ViReal64 Tmp;
	checkErr(viPrintf(vi, "ON %d,%d\n", (Chan+1), NumRds));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	for(i=0; i<NumRds; i++)
	{
		Scan(Buf+(i*(sizeof(ViReal64)+1)), "%f", &Tmp);
		Rslt[i] = Tmp;
	}
	
Error:
	return error;
}

/****************************************************************************/
/*			              OUTPUT ONE READING FUNCTION      	        		*/
/*					          												*/
/*	Outputs one channel reading.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputOneReading(ViSession vi, ViInt32 Chan, 
										  ViReal64 *Rslt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[30];
	ViInt32 Bufsize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "O %d\n", (Chan+1)));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%f", Rslt));

Error:
	return error;
}

/****************************************************************************/
/*			         OUTPUT VALID SAMPLE ARRAY FUNCTION         	  		*/
/*					          												*/
/*	Outputs a sample array.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_OutputValidSampleArray(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "OGSD\n")); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           PEAK METER DISPLAY SET FUNCTION             	  		*/
/*					          												*/
/*	Sets the peak meter display to off, A, B, or A&B.						*/
/*																			*/
/****************************************************************************/
									    //enum PeakDisplay  
ViStatus _VI_FUNC ML2430_SetPeakMeterDisplay(ViSession vi, ViInt32 PkDsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[4][4] = {"OFF","A","B","A&B"};
	checkErr(viPrintf(vi, "DPEAK %s\n", Matrix[PkDsp])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           PEAK METER DISPLAY GET FUNCTION             	  		*/
/*					          												*/
/*	Returns whether the peak meter display is set to off, A, B, or A&B.		*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetPeakMeterDisplay(ViSession vi, ViInt32 *PkDsp)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "DPEAK?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	*PkDsp = PkDsp_Off;
	if(strstr(Buf, "OFF")==NULL)
		if(strstr(Buf, "A&B")==NULL)
			if(strstr(Buf, "B")==NULL)
				*PkDsp = PkDsp_A;
			else
				*PkDsp = PkDsp_B;
		else
			*PkDsp = PkDsp_AB;

Error:
	return error;
}

/****************************************************************************/
/*			                     PRINT FUNCTION                 	  		*/
/*					          												*/
/*	Prints.																	*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Print(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "PRINT\n")); 		  

Error:
	return error;
}

/****************************************************************************/
/*			             PRINTER SELECT SET FUNCTION             	  		*/
/*					          												*/
/*	Selects the printer.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetPrinterSelect(ViSession vi, ViInt32 Slct)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][6] = {"HP340","BJC80"};
	checkErr(viPrintf(vi, "PRNSEL %s\n", Matrix[Slct])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			             PRINTER SELECT GET FUNCTION             	  		*/
/*					          												*/
/*	Returns the selected printer.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetPrinterSelect(ViSession vi, ViInt32 *Slct)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "PRNSEL?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Slct = Printer_HP340;
	if(strstr(Buf, "HP340")==NULL)
		*Slct = Printer_BJC80;

Error:
	return error;
}

/****************************************************************************/
/*			        PROFILE CHANNEL SELECTION SET FUNCTION         	  		*/
/*					          												*/
/*	Selects the profile channel.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileChannelSelection(ViSession vi, ViInt32 Chan)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GRMD %d\n", (Chan+1))); 		  

Error:
	return error;
}

/****************************************************************************/
/*			        PROFILE CHANNEL SELECTION GET FUNCTION         	  		*/
/*					          												*/
/*	Returns the profile channel.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileChannelSelection(ViSession vi, ViInt32 *Chan)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GRMD?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Chan = Channel_1;
	if(strstr(Buf, "1")==NULL)
		*Chan = Channel_2;

Error:
	return error;
}

/****************************************************************************/
/*			         PROFILE CONNECT POINTS SET FUNCTION         	  		*/
/*					          												*/
/*	Sets profile connect points on or off.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileConnectPoints(ViSession vi, ViInt32 ConPts)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "GRCP %s;\n", Matrix[ConPts])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			         PROFILE CONNECT POINTS GET FUNCTION         	  		*/
/*					          												*/
/*	Returns whether profile connect points is on or off.					*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileConnectPoints(ViSession vi, ViInt32 *ConPts)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GRCP?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*ConPts = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*ConPts = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE DATA COLLECTION PERIOD SET FUNCTION      	  		*/
/*					          												*/
/*	Sets the profile data collection period.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileDataCollectionPeriod(ViSession vi, 
														ViReal64 CollPd)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GRPRD %.7f\n", CollPd)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE DATA COLLECTION PERIOD GET FUNCTION      	  		*/
/*					          												*/
/*	Returns the profile data collection period.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileDataCollectionPeriod(ViSession vi, 
														ViReal64 *CollPd)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GRPRD?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "GRPRD %f", CollPd));

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE EXTERNAL TRIGGER EDGE SET FUNCTION      	  		*/
/*					          												*/
/*	Sets the profile external trigger edge.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileExternalTriggerEdge(ViSession vi, 
													   ViInt32 Trig)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][5] = {"RISE","FALL"};
	checkErr(viPrintf(vi, "GTXTTL 1&2,%s\n", Matrix[Trig])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE EXTERNAL TRIGGER EDGE GET FUNCTION      	  		*/
/*					          												*/
/*	Returns the profile external trigger edge.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileExternalTriggerEdge(ViSession vi, 
													   ViInt32 *Trig)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GTXTTL?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Trig = Profile_Rise;
	if(strstr(Buf, "RISE")==NULL)
		*Trig = Profile_Fall;

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE MIN MAX TRACKING MODE SET FUNCTION      	  		*/
/*					          												*/
/*	Sets the profile min max tracking mode.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileMinMaxTrackingMode(ViSession vi, 
													  ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][9] = {"SINGLE","INFINITE"};
	checkErr(viPrintf(vi, "GRTMM %s\n", Matrix[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE MIN MAX TRACKING MODE GET FUNCTION      	  		*/
/*					          												*/
/*	Returns the profile min max tracking mode.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileMinMaxTrackingMode(ViSession vi, 
													   ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GRTMM?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Mode = Profile_Single;
	if(strstr(Buf, "SINGLE")==NULL)
		*Mode = Profile_Infinite;

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE REFERENCE LINE STATE SET FUNCTION      	  		*/
/*					          												*/
/*	Sets the profile reference line on or off.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileReferenceLineState(ViSession vi, 
													  ViInt32 Lne)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "GRFS %s;\n", Matrix[Lne])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			      PROFILE REFERENCE LINE STATE GET FUNCTION      	  		*/
/*					          												*/
/*	Returns whether the profile reference line is on or off.				*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileReferenceLineState(ViSession vi, 
													   ViInt32 *Lne)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GRFS?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Lne = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Lne = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			                PROFILE RESET FUNCTION                 	  		*/
/*					          												*/
/*	Sets the profile reset mode.																	*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ProfileReset(ViSession vi, ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][6] = {"CRCLR","CRSWR"};
	checkErr(viPrintf(vi, "%s\n", Matrix[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          PROFILE TRIGGER ARMING SET FUNCTION       	  		*/
/*					          												*/
/*	Sets the profile trigger to armed (on) or unarmed (off).				*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileTriggerArming(ViSession vi, ViInt32 St)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "GTARM %s;\n", Matrix[St])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          PROFILE TRIGGER ARMING GET FUNCTION       	  		*/
/*					          												*/
/*	Returns whether the profile trigger is on or off.						*/
/*																			*/
/****************************************************************************/
							
ViStatus _VI_FUNC ML2430_GetProfileTriggerArming(ViSession vi, ViInt32 *St)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GTARM?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*St = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*St = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			           PROFILE TRIGGER DELAY SET FUNCTION       	  		*/
/*					          												*/
/*	Sets the profile trigger delay.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileTriggerDelay(ViSession vi, ViReal64 Dly)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GTDLY %f\n", Dly)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           PROFILE TRIGGER DELAY GET FUNCTION       	  		*/
/*					          												*/
/*	Returns the profile trigger delay.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileTriggerDelay(ViSession vi, ViReal64 *Dly)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GTDLY?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "GTDLY %f", Dly));

Error:
	return error;
}

/****************************************************************************/
/*			        PROFILE TRIGGER GATE WIDTH SET FUNCTION       	  		*/
/*					          												*/
/*	Sets the profile trigger gate width.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileTriggerGateWidth(ViSession vi, ViReal64 Wid)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GTGW %.7f\n", Wid)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			        PROFILE TRIGGER GATE WIDTH GET FUNCTION       	  		*/
/*					          												*/
/*	Returns the profile trigger gate width.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileTriggerGateWidth(ViSession vi, ViReal64 *Wid)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GTGW?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "GTGW %f", Wid));

Error:
	return error;
}

/****************************************************************************/
/*			          PROFILE TRIGGER LEVEL SET FUNCTION        	  		*/
/*					          												*/
/*	Sets the profile trigger level.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileTriggerLevel(ViSession vi, ViReal64 Trig)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GTLVL %f\n", Trig)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          PROFILE TRIGGER LEVEL GET FUNCTION        	  		*/
/*					          												*/
/*	Returns the profile trigger level.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileTriggerLevel(ViSession vi, ViReal64 *Trig)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GTLVL?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "GTLVL %f", Trig));

Error:
	return error;
}

/****************************************************************************/
/*			          PROFILE TRIGGER SOURCE SET FUNCTION        	  		*/
/*					          												*/
/*	Sets the profile trigger source.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileTriggerSource(ViSession vi, ViInt32 Src)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[5][7] = {"INTA","INTB","EXTTTL","MANUAL","CONT"};
	checkErr(viPrintf(vi, "GTSRC %s\n", Matrix[Src])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          PROFILE TRIGGER SOURCE GET FUNCTION        	  		*/
/*					          												*/
/*	Returns the profile trigger source.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileTriggerSource(ViSession vi, ViInt32 *Src)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GTSRC?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	*Src = Profile_INTA;
	if(strstr(Buf,"INTA")==NULL)
		if(strstr(Buf,"INTB")==NULL)
			if(strstr(Buf,"EXTTTL")==NULL)
				if(strstr(Buf,"MANUAL")==NULL)
					*Src = Profile_CONT;
				else
					*Src = Profile_MANUAL;
			else
				*Src = Profile_EXTTTL;
		else
			*Src = Profile_INTB;

Error:
	return error;
}

/****************************************************************************/
/*			           PROFILE TRIGGER TYPE SET FUNCTION        	  		*/
/*					          												*/
/*	Sets the profile trigger type.											*/
/*																			*/
/****************************************************************************/
									    //enum ProfileTrigger  
ViStatus _VI_FUNC ML2430_SetProfileTriggerType(ViSession vi, ViInt32 Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][5] = {"RISE","FALL"};
	checkErr(viPrintf(vi, "GTTYP %s\n", Matrix[Trg])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           PROFILE TRIGGER TYPE GET FUNCTION        	  		*/
/*					          												*/
/*	Returns the profile trigger type.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileTriggerType(ViSession vi, ViInt32 *Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GTTYP?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Trg = Profile_Rise;
	if(strstr(Buf, "RISE")==NULL)
		*Trg = Profile_Fall;

Error:
	return error;
}

/****************************************************************************/
/*			               PROFILE TYPE SET FUNCTION            	  		*/
/*					          												*/
/*	Sets the profile type.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileType(ViSession vi, ViInt32 Typ)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[5][7] = {"NORM","MINMAX","MIN","MAX","AVG"};
	checkErr(viPrintf(vi, "GRPIX %s\n", Matrix[Typ])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			               PROFILE TYPE GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the profile type.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileType(ViSession vi, ViInt32 *Typ)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GRPIX?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	*Typ = Profile_NORM;
	if(strstr(Buf, "NORM")==NULL)
		if(strstr(Buf, "MINMAX")==NULL)
			if(strstr(Buf, "MIN")==NULL)
				if(strstr(Buf, "MAX")==NULL)
					*Typ = Profile_AVG;
				else
					*Typ = Profile_MAX;
			else
				*Typ = Profile_MIN;
		else
			*Typ = Profile_MINMAX;

Error:
	return error;
}

/****************************************************************************/
/*			           PROFILE Y-AXIS SCALE SET FUNCTION          	  		*/
/*					          												*/
/*	Sets the profile Y-axis scale limits.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetProfileYAxisScale(ViSession vi, ViReal64 ULim,
											  ViReal64 LLim)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GRYT %.2f;GRYB %.2f\n", ULim, LLim)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           PROFILE Y-AXIS SCALE GET FUNCTION           	  		*/
/*					          												*/
/*	Returns the profile Y-axis scale limits.								*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetProfileYAxisScale(ViSession vi, ViReal64 *ULim,
											  ViReal64 *LLim)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;

	checkErr(viPrintf(vi, "GRYB?\n"));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;
	checkErr(Scan(Buf, "GRYB %f", LLim));
	checkErr(viPrintf(vi, "GRYT?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;
	checkErr(Scan(Buf, "GRYT %f", ULim));

Error:
	return error;
}

/****************************************************************************/
/*			         RANGE CALIBRATOR DATA QUERY FUNCTION          	  		*/
/*					          												*/
/*	Returns the calibrator data.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_RangeCalibratorDataQuery(ViSession vi, ViInt32 Snsr,
												  ViBoolean *St, ViInt32 *ZerVal, 
												  ViInt32 Up[5], ViInt32 Low[5])
{
	ViStatus error = VI_SUCCESS;
	ViInt32 TmpArray[11], i;
	ViChar Buf[200];
	ViUInt32 BufSize = sizeof(Buf)-1;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "RCD %s\n", Temp[Snsr])); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	if(strstr(Buf, "FALSE")==NULL)
	{	
		*St = 1;
		for(i=0; i<11; i++)
			Scan(Buf+10+i*5, "%d", TmpArray[i]);
		*ZerVal = TmpArray[0];
		for(i=1; i<11; i++)
		{
			if((i%2)==0)
				Low[((i/2)-1)] = TmpArray[i];
			else
				Up[((i-1)/2)] = TmpArray[i];
		}
	}
	else
	{
		*St = 0;
		*ZerVal = 0;
		for(i=0; i<5; i++)
		{
			Low[i] = 0;
			Up[i] = 0;
		}
	}
	
Error:
	return error;
}

/****************************************************************************/
/*			               REDIAL COUNT SET FUNCTION            	  		*/
/*					          												*/
/*	Sets the redial count.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetRedialCount(ViSession vi, ViUInt8 Rdl)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "MODRED %d\n", Rdl)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			               REDIAL COUNT GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the redial count.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetRedialCount(ViSession vi, ViUInt8 *Rdl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "MODRED?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "MODRED %d", &Tmp));
	*Rdl = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			            RELATIVE CONTROL SET FUNCTION            	  		*/
/*					          												*/
/*	Sets the relative control mode.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetRelativeControl(ViSession vi, ViInt32 Chan,
											ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "REL %d,%d\n", (Chan+1), Mode)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            RELATIVE CONTROL GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the relative control mode.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetRelativeControl(ViSession vi, ViInt32 Chan,
											ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "REL? %d\n", (Chan+1)));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t44q],%d", 19, Tmp, Mode));

Error:
	return error;
}

/****************************************************************************/
/*			            RESET MIN MAX GRAPH FUNCTION            	  		*/
/*					          												*/
/*	Resets the min/max graph.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ResetMinMaxGraph(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GPRST\n")); 		  

Error:
	return error;
}

/****************************************************************************/
/*			                    RESET FUNCTION                  	  		*/
/*					          												*/
/*	Resets the instrument, except the offset tables, GPIB address, queues,  */
/*  and status registers.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_Reset(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*RST\n")); 		  

Error:
	return error;
}

/****************************************************************************/
/*			               RESET TO FACTORY FUNCTION               	  		*/
/*					          												*/
/*	Resets the entire instrument to the factory preset.						*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ResetToFactory(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "FRST\n")); 		  

Error:
	return error;
}

/****************************************************************************/
/*			         RF REF CALIBRATOR ON-OFF SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the RF ref calibrator on or off.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetRFRefCalibratorOnOff(ViSession vi, ViInt32 Cal)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] ={"OFF","ON"};
	checkErr(viPrintf(vi, "RFCAL %s\n", Matrix[Cal])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			         RF REF CALIBRATOR ON-OFF GET FUNCTION         	  		*/
/*					          												*/
/*	Returns whether the RF ref calibrator is on or off.						*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetRFRefCalibratorOnOff(ViSession vi, ViInt32 *Cal)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "RFCAL?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Cal = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Cal = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			           RS232 BAUD RATE SET FUNCTION               	  		*/
/*					          												*/
/*	Sets the baud rate.														*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetRS232BaudRate(ViSession vi, ViInt32 Rate)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[6][4] = {"12","24","48","96","192","384"};
	checkErr(viPrintf(vi, "RSBAUD %s\n", Matrix[Rate])); 		  
										  
Error:
	return error;
}

/****************************************************************************/
/*			            RS232 BAUD RATE GET FUNCTION               	  		*/
/*					          												*/
/*	Returns the baud rate.													*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetRS232BaudRate(ViSession vi, ViInt32 *Rate)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[6][4] = {"12","24","48","96","192","384"};
	ViChar Buf[100];
	ViInt32 TmpRate, Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "RSBAUD?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "RSBAUD %d", &TmpRate));
	*Rate = (log10((TmpRate/12)))/(log10(2));

Error:
	return error;
}

/****************************************************************************/
/*			          RS232 OPERATING MODE SET FUNCTION           	  		*/
/*					          												*/
/*	Sets the RS232 operating mode.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetRS232OperatingMode(ViSession vi, ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][7] = {"EXTCOM","SRCSWP"};
	checkErr(viPrintf(vi, "RSMODE %s\n", Matrix[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			          RS232 OPERATING MODE GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the RS232 operating mode.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetRS232OperatingMode(ViSession vi, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "RSMODE?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Mode = Mode_ExtComm;
	if(strstr(Buf, "EXTCOM")==NULL)
		*Mode = Mode_SrcSwp;

Error:
	return error;
}

/****************************************************************************/
/*			            SAVED SET NAMING SET FUNCTION           	  		*/
/*					          												*/
/*	Sets the saved set naming.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSavedSetNaming(ViSession vi, ViUInt8 StNum,
										   ViChar Txt[])
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "SYSLNM %d,%s\n", StNum, Txt)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            SAVED SET NAMING GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the saved set naming.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetSavedSetNaming(ViSession vi, ViUInt8 StNum,
										   ViChar Txt[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	
	checkErr(viPrintf(vi, "SYSLNM? %d\n", StNum)); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "SYSLNM %d,%s\n", &Tmp, Txt));

Error:
	return error;
}

/****************************************************************************/
/*			               SAVED SETUP LOAD FUNCTION             	  		*/
/*					          												*/
/*	Sets the saved setup.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_LoadSavedSetup(ViSession vi, ViUInt16 StNum,
									    ViChar Data[], ViInt32 Bytes)
{
	ViStatus error = VI_SUCCESS;
	ViChar *pBuf;
	ViInt32 Len,BytesSent;
	pBuf=malloc(50+Bytes);
	
	if(StNum<1)
		StNum = 1;
	sprintf(pBuf, "SYSLD %d,%d,", StNum, Bytes);
	Len = strlen(pBuf);
	memcpy(pBuf+Len, Data, Bytes);
	checkErr(viWrite(vi, pBuf, Len+Bytes, &BytesSent));

Error:
	free(pBuf);
	return error;
}

/****************************************************************************/
/*			               SAVED SETUP READ FUNCTION              	  		*/
/*					          												*/
/*	Returns the saved setup.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_ReadSavedSetup(ViSession vi, ViUInt16 StNum,
									    ViInt32 DataSize,ViChar Data[],
									    ViInt32 *Bytes)
{
	ViStatus error = VI_SUCCESS;
	ViChar Header[20];
	ViInt32 BytesLeft,BytesRead;
	ViBoolean Truncate=VI_FALSE;
	*Bytes=0;

	checkErr(viPrintf(vi, "SYSRD %d\n", StNum));

	checkErr(viRead(vi,Header,8,&BytesRead));
	checkErr(viRead(vi,Header,Header[7]-'0'+1,&BytesRead));
	Header[BytesRead-1]=0x00;
	BytesLeft=atoi(Header)+1;
	if (BytesLeft>DataSize)
	{
		Truncate=VI_TRUE;
		BytesLeft=DataSize;
	}

	while (BytesLeft>0)
	{
		checkErr(viRead(vi,Data+*Bytes,1000,&BytesRead));
		BytesLeft-=BytesRead;
		*Bytes+=BytesRead;
	}
	--*Bytes;
	
	if (Truncate==VI_TRUE)
		viClear(vi);
	
Error:
	return error;
}

/****************************************************************************/
/*			           SECURE SYSTEM STATE SET FUNCTION           	  		*/
/*					          												*/
/*	Sets the secure system state to on or off.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSecureSystemState(ViSession vi, ViInt32 St)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "SECURE %s\n", Matrix[St])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			           SECURE SYSTEM STATE GET FUNCTION            	  		*/
/*					          												*/
/*	Returns whether the secure system state is set to on or off.			*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetSecureSystemState(ViSession vi, ViInt32 *St)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViUInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "SECURE?\n")); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*St = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*St = Switch_Off;

Error:
	return error;
}

/****************************************************************************/
/*			          SELECT SENSOR OFFSET SET FUNCTION           	  		*/
/*					          												*/
/*	Selects the sensor offset.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSelectSensorOffset(ViSession vi, ViInt32 Snsr,
											   ViInt32 Typ)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[3][6] = {"OFF","FIXED","TABLE"};
	checkErr(viPrintf(vi, "OFFTYP %s,%s\n", Temp[Snsr], Matrix[Typ])); 

Error:
	return error;
}

/****************************************************************************/
/*			          SELECT SENSOR OFFSET GET FUNCTION            	  		*/
/*					          												*/
/*	Returns the sensor offset.												*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetSelectSensorOffset(ViSession vi, ViInt32 Snsr,
											   ViInt32 *Typ)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"}, Buf[100], Tmp[20], Tmp2[10];
	ViInt32 Bufsize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "OFFTYP? %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%s", 19, Tmp, 19, Tmp, Tmp2));
	*Typ = Type_Off;
	if(strstr(Tmp2,"OFF")==NULL)
		if(strstr(Tmp2,"TABLE")==NULL)
			*Typ = Type_Fixed;
		else
			*Typ = Type_Table;
	
Error:
	return error;
}

/****************************************************************************/
/*			              SELF TEST STATUS FUNCTION              	  		*/
/*					          												*/
/*	Returns the self test status.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_SelfTestStatus(ViSession vi, ViChar Flsh[5], 
										ViChar CalDat[5], ViChar Prsnlty[5], 
										ViChar Ram[5], ViChar Nonvol[5], 
										ViChar Lcd[5], ViChar Kbd[5], 
										ViChar Dsp[5])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[2000];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "STERR\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "FLASH 0x%s[w*t44q],CALDAT 0x%s[w*t44q],PERSON 0x%s[w*t44q],RAM 0x%s[w*t44q],NONVOL 0x%s[w*t44q],LCD 0x%s[w*t44q],KBD 0x%s[w*t44q],DSP 0x%s[w*t44q]",
				  4, Flsh, 4, CalDat, 4, Prsnlty, 4, Ram, 4, Nonvol, 4, Lcd, 4,
				  Kbd, 4, Dsp));

Error:
	return error;
}

/****************************************************************************/
/*			             SENSOR INFORMATION FUNCTION            	  		*/
/*					          												*/
/*	Returns the sensor information.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_SensorInformation(ViSession vi, ViInt32 Snsr,
										   ViInt32 InfoSize,ViChar Info[])
{
	ViStatus error = VI_SUCCESS;
	ViInt32 BytesRead;
	ViInt32 Bufsize;
	ViChar Temp[2][2] = {"A","B"};
	Bufsize = sizeof(Info)-1;
	checkErr(viPrintf(vi, "SENTYP %s\n", Temp[Snsr])); 
	checkErr(viRead(vi, Info, Bufsize, &Bufsize));
	Info[Bufsize] = 0x00;
	
Error:
	return error;
}

/****************************************************************************/
/*			         SENSOR MEASUREMENT MODE SET FUNCTION          	  		*/
/*					          												*/
/*	Selects the sensor measurement mode.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSensorMeasurementMode(ViSession vi, ViInt32 Snsr,
											   	  ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Matrix[3][8] = {"DEFAULT", "MOD", "CUSTOM"};
	checkErr(viPrintf(vi, "SENMM %s,%s\n", Temp[Snsr], Matrix[Mode])); 		  

Error:
	return error;
}

/****************************************************************************/
/*			         SENSOR MEASUREMENT MODE GET FUNCTION          	  		*/
/*					          												*/
/*	Returns the sensor measurement mode.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetSensorMeasurementMode(ViSession vi, ViInt32 Snsr,
											   	  ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "SENMM? %s\n", Temp[Snsr])); 
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;
	*Mode = Mode_DEFAULT;
	
	if(strstr(Buf, "DEFAULT")==NULL)
		if(strstr(Buf, "MOD")==NULL)
			*Mode = Mode_CUSTOM;
		else
			*Mode = Mode_MOD;
	
Error:
	return error;
}

/****************************************************************************/
/*			              SENSOR OFFSET VALUE FUNCTION          	  		*/
/*					          												*/
/*	Returns the sensor offset value.										*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_SensorOffsetValue(ViSession vi, ViInt32 Snsr,
										   ViReal64 *Val)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"}, Buf[100], Tmp[20];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "OFFVAL %s\n", Temp[Snsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%f", Val));
	
Error:
	return error;
}

/****************************************************************************/
/*			            SENSOR RANGE HOLD SET FUNCTION          	  		*/
/*					          												*/
/*	Selects the sensor range hold.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSensorRangeHold(ViSession vi, ViInt32 Snsr,
											ViUInt8 Tbl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "RGH %s,%d\n", Temp[Snsr], Tbl)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			            SENSOR RANGE HOLD GET FUNCTION          	  		*/
/*					          												*/
/*	Returns the sensor range hold.											*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetSensorRangeHold(ViSession vi, ViInt32 ISnsr,
											ViInt32 *OSnsr, ViUInt8 *Tbl)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar Tmp[20], TmpTbl[8], Buf[200];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "RGH? %s\n", Temp[ISnsr])); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;
	
	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%s", 19, Tmp, 19, Tmp, TmpTbl));
	
	*OSnsr = SensorA;
	if(strstr(Tmp,"A")==NULL)
		*OSnsr = SensorB;
	*Tbl = 0;
	if(strstr(TmpTbl, "AUTO")==NULL)
		*Tbl = TmpTbl[0] - 48;
	
Error:
	return error;
}

/****************************************************************************/
/*			         SENSOR SETTLE PERCENTAGE SET FUNCTION         	  		*/
/*					          												*/
/*	Sets the sensor settle percentage.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSensorSettlePercentage(ViSession vi, ViInt32 Snsr,
												   ViReal64 Pct)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "SENSTL %1s,%lf\n", Temp[Snsr], Pct)); 		  

Error:
	return error;
}

/****************************************************************************/
/*			         SENSOR SETTLE PERCENTAGE GET FUNCTION         	  		*/
/*					          												*/
/*	Returns the sensor settle percentage.									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_GetSensorSettlePercentage(ViSession vi, ViInt32 ISnsr,
												   ViInt32 *OSnsr,
												   ViReal64 *Pct)
{
	ViStatus error = VI_SUCCESS;
	ViChar Temp[2][2] = {"A","B"};
	ViChar TmpChr[20];
	
	ViChar Buf[200];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "SENSTL? %1s\n", Temp[ISnsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%f",19,TmpChr,19,TmpChr,Pct));
	
	*OSnsr = SensorA;
	if(strstr(Buf, "A")==NULL)
		*OSnsr = SensorB;

Error:
	return error;
}

/****************************************************************************/
/*			   SET QUERY EVENT STATUS ENABLE REGISTER FUNCTION     	 		*/
/*					          												*/
/*	Sets the event status enable register based on the eight bits: 			*/
/*  Operation Complete, Request Control, Query Error, Device Dependent      */
/*  Error, Execution Error, Command Error, User Request, and Power On. 		*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetQueryEventStatusRegister(ViSession vi, ViUInt8 Inpt)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "*ESE %d\n", Inpt)); 

Error:
	return error;
}

/****************************************************************************/
/*			   GET QUERY EVENT STATUS ENABLE REGISTER FUNCTION     	 		*/
/*					          												*/
/*	Returns the event status enable register based on the eight bits:		*/
/*  Operation Complete, Request Control, Query Error, Device Dependent      */
/*  Error, Execution Error, Command Error, User Request, and Power On. 		*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetQueryEventStatusRegister(ViSession vi,
													 ViUInt8 *Inpt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[40];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	checkErr(viPrintf(vi, "*ESE?\n")); 
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp));
	*Inpt = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			        SET QUERY SRQ ENABLE REGISTER FUNCTION       	 		*/
/*					          												*/
/*	Sets the SRQ enable register based on the eight bits: 					*/
/*  Operation Complete, Request Control, Query Error, Device Dependent      */
/*  Error, Execution Error, Command Error, User Request, and Power On. 		*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetQuerySRQEnableRegister(ViSession vi, ViUInt8 Inpt)
{
	ViStatus error = VI_SUCCESS;
	if((Inpt & ML2430_RQS)==ML2430_RQS)
		Inpt = Inpt - ML2430_RQS;
	checkErr(viPrintf(vi, "*SRE %d\n", Inpt)); 

Error:
	return error;
}

/****************************************************************************/
/*			        GET QUERY SRQ ENABLE REGISTER FUNCTION       	 		*/
/*					          												*/
/*	Returns the SRQ enable register based on the eight bits: 				*/
/*  Operation Complete, Request Control, Query Error, Device Dependent      */
/*  Error, Execution Error, Command Error, User Request, and Power On. 		*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetQuerySRQEnableRegister(ViSession vi, ViUInt8 *Inpt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[40];
	ViInt32 Bufsize = sizeof(Buf)-1, Tmp;
	checkErr(viPrintf(vi, "*SRE?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &Tmp));
	*Inpt = Tmp;

Error:
	return error;
}

/****************************************************************************/
/*			           SETUP BNC ANALOG OUTPUT FUNCTION         	 		*/
/*					          												*/
/*	Configures the setup for BNC analog output.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetupBNCAnalogOutput(ViSession vi, ViInt32 Prt,
											  ViReal64 StVolt, ViReal64 SpVolt,
											  ViInt32 StValUnit, ViReal64 StVal, 
											  ViInt32 SpValUnit, ViReal64 SpVal)
{
	ViStatus error = VI_SUCCESS;
	checkErr(ML2430_SetBNCAnalogOutputStartVolt(vi, Prt, StVolt));
	checkErr(ML2430_SetBNCAnalogOutputStopVolt(vi, Prt, SpVolt));
	checkErr(ML2430_SetBNCAnalogOutDisplayStart(vi, Prt, StValUnit, StVal));
	checkErr(ML2430_SetBNCAnalogOutDisplayStop(vi, Prt, SpValUnit, SpVal));
	
Error:
	return error;
}

/****************************************************************************/
/*			           SETUP CAL FACTOR CONFIG FUNCTION         	 		*/
/*					          												*/
/*	Configures the calibration setup.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetupCalFactorConfig(ViSession vi, ViInt32 Snsr,
											  ViInt32 Src, ViReal64 CalFrq, 
											  ViInt32 FrqUnit, ViReal64 AdjVal,
											  ViInt32 AdjUnit, ViReal64 ManVal,
											  ViInt32 ManUnit, ViInt32 Tbl)
{
	ViStatus error = VI_SUCCESS;
	checkErr(ML2430_SetCalSource(vi, Snsr, Src));
	checkErr(ML2430_SetCalFrequency(vi, Snsr, FrqUnit, CalFrq));
	checkErr(ML2430_SetCalAdjust(vi, Snsr, AdjUnit, AdjVal));
	checkErr(ML2430_SetCalManual(vi, Snsr, ManUnit, ManVal));
	checkErr(ML2430_CalTableSelect(vi, Snsr, Tbl));
	
Error:
	return error;
}

/****************************************************************************/
/*			             SETUP CHANNEL CONFIG FUNCTION          	 		*/
/*					          												*/
/*	Configures the channel setup.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetupChannelConfig(ViSession vi, ViInt32 Chan,
											ViInt32 Cnfig, ViInt32 Trck, 
											ViInt32 Unit, ViInt32 Res)
{
	ViStatus error = VI_SUCCESS;
	checkErr(ML2430_SetChannelInputConfig(vi, Chan, Cnfig));
	checkErr(ML2430_SetMinMaxTracking(vi, Chan, Trck));
	checkErr(ML2430_SetChannelUnits(vi, Chan, Unit));
	checkErr(ML2430_SetChannelResolution(vi, Chan, Res));
	
Error:
	return error;
}

/****************************************************************************/
/*			                 SETUP LIMITS FUNCTION              	 		*/
/*					          												*/
/*	Configures the limits setup.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetupLimits(ViSession vi, ViInt32 Chan, ViReal64 HLim,
									 ViReal64 LLim, ViInt32 HLoo, ViInt32 LLoo, 
									 ViInt32 FlBp, ViInt32 FlIndHld)
{
	ViStatus error = VI_SUCCESS;
	checkErr(ML2430_SetLimitsSetting(vi, Chan, HLim, LLim));
	checkErr(ML2430_SetLimitsOnOff(vi, Chan, HLoo, LLoo));
	checkErr(ML2430_SetFailBeepOnOff(vi, Chan, FlBp));
	checkErr(ML2430_SetFailIndicatorHold(vi, Chan, FlIndHld));
	
Error:
	return error;
}

/****************************************************************************/
/*			            SETUP SENSOR AVERAGE FUNCTION              	 		*/
/*					          												*/
/*	Configures the sensor average setup.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetupSensorAverage(ViSession vi, ViInt32 Snsr, 
											ViInt32 Typ, ViUInt16 NumRd, 
											ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	checkErr(ML2430_SetAveragingSetup(vi, Snsr, Typ, NumRd));
	checkErr(ML2430_SetAveragingAutoLowLevel(vi, Snsr, Mode));
	
Error:
	return error;
}

/****************************************************************************/
/*			            SETUP SENSOR CONFIG FUNCTION              	 		*/
/*					          												*/
/*	Configures the sensor setup.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetupSensorConfig(ViSession vi, ViInt32 Snsr, 
										   ViInt32 Mode, ViReal32 Pctg,
										   ViUInt8 Tbl)
{
	ViStatus error = VI_SUCCESS;
	checkErr(ML2430_SetSensorMeasurementMode(vi, Snsr, Mode));
	checkErr(ML2430_SetSensorSettlePercentage(vi, Snsr, Pctg));
	checkErr(ML2430_SetSensorRangeHold(vi, Snsr, Tbl));
	
Error:
	return error;
}

/****************************************************************************/
/*			            SETUP TRIGGER CONFIG FUNCTION              	 		*/
/*					          												*/
/*	Configures the trigger setup.											*/
/*																			*/
/****************************************************************************/
									    //enum ProfileSource  
ViStatus _VI_FUNC ML2430_SetupTriggerConfig(ViSession vi, ViInt32 RdOut, 
										    ViInt32 Chan, ViInt32 Src,
										    ViReal64 GtWid, ViReal64 DlyVal,
										    ViInt32 TrgArm, ViInt32 TrgTyp,
										    ViReal64 TrgLvl, ViInt32 ExTrgTyp)
{
	ViStatus error = VI_SUCCESS;
	if(RdOut==PR_Profile)
	{
		checkErr(ML2430_SetProfileTriggerSource(vi, Src));
		checkErr(ML2430_SetProfileTriggerGateWidth(vi, GtWid));
		checkErr(ML2430_SetProfileTriggerDelay(vi, DlyVal));
		checkErr(ML2430_SetProfileTriggerArming(vi, TrgArm));
		checkErr(ML2430_SetProfileTriggerType(vi, TrgTyp));
		checkErr(ML2430_SetProfileTriggerLevel(vi, TrgLvl));
		checkErr(ML2430_SetProfileExternalTriggerEdge(vi, ExTrgTyp));
	}
	else
	{
		checkErr(ML2430_SetTriggerSource(vi, Chan, Src));
		checkErr(ML2430_SetTriggerGateWidth(vi, Chan, GtWid));
		checkErr(ML2430_SetTriggerDelay(vi, Chan, DlyVal));
		checkErr(ML2430_SetTriggerArming(vi, Chan, TrgArm));
		checkErr(ML2430_SetTriggerType(vi, Chan, TrgTyp));
		checkErr(ML2430_SetTriggerLevel(vi, Chan, TrgLvl));
		checkErr(ML2430_SetTriggerExternalEdge(vi, Chan, ExTrgTyp));
	}
	
Error:
	return error;
}

/****************************************************************************/
/*			       SOURCE SWEEP MODE STATUS REQUEST FUNCTION       	 		*/
/*					          												*/
/*	Returns the source sweep status.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SourceSweepModeStatusRequest(ViSession vi,
													  ViInt32 *Mode,
													  ViReal64 *StPow, 
													  ViReal64 *SpPow,
													  ViReal64 *StFrq,
													  ViReal64 *SpFrq)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[10], Buf[100];
	checkErr(viPrintf(vi, "SRCSTST\n"));
	checkErr(Scan(Buf, "%s[w*t44q],%f,%f,%f,%f", TmpStr, &StPow, &SpPow, &StFrq, 
				  &SpFrq));
	*Mode = Mode_POWER;
	if(strstr(Buf, "FREQ")!=NULL)
		*Mode = Mode_FREQ;
	
Error:
	return error;
}

/****************************************************************************/
/*                       SOURCE SWEEP MODE SET FUNCTION  	     	 		*/
/*					          												*/
/*	Sets the source sweep mode.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSourceSweepMode(ViSession vi, ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][6] = {"FREQ","POWER"};
	checkErr(viPrintf(vi, "SRCMOD %s\n", Matrix[Mode]));
	
Error:
	return error;
}

/****************************************************************************/
/*                       SOURCE SWEEP MODE GET FUNCTION  	     	 		*/
/*					          												*/
/*	Returns the source sweep mode.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSourceSweepMode(ViSession vi, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "SRCMOD?\n"));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Mode = Mode_FREQ;
	if(strstr(Buf, "FREQ")==NULL)
		*Mode = Mode_POWER;
	
Error:
	return error;
}

/****************************************************************************/
/*                SOURCE SWEEP START FREQUENCY SET FUNCTION  	  	 		*/
/*					          												*/
/*	Sets the source sweep start frequency.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSourceSweepStartFrequency(ViSession vi, 
													  ViReal64 Frq)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "SRCSTFRQ %f\n", Frq));
	
Error:
	return error;
}

/****************************************************************************/
/*                SOURCE SWEEP START FREQUENCY GET FUNCTION      	 		*/
/*					          												*/
/*	Returns the source sweep start frequency.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSourceSweepStartFrequency(ViSession vi, 
													  ViReal64 *Frq)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "SRCSTFRQ?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;
	checkErr(Scan(Buf, "SRCSTFRQ %f", Frq));
	
Error:
	return error;
}

/****************************************************************************/
/*                  SOURCE SWEEP START POWER SET FUNCTION  	    	 		*/
/*					          												*/
/*	Sets the source sweep start power.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSourceSweepStartPower(ViSession vi, ViReal64 Pow)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "SRCSTPWR %f\n", Pow));
	
Error:
	return error;
}

/****************************************************************************/
/*                  SOURCE SWEEP START POWER GET FUNCTION        	 		*/
/*					          												*/
/*	Returns the source sweep start power.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSourceSweepStartPower(ViSession vi, ViReal64 *Pow)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "SRCSTPWR?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "SRCSTPWR %f", Pow));
	
Error:
	return error;
}

/****************************************************************************/
/*                SOURCE SWEEP STOP FREQUENCY SET FUNCTION  	  	 		*/
/*					          												*/
/*	Sets the source sweep stop frequency.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSourceSweepStopFrequency(ViSession vi, 
													  ViReal64 Frq)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "SRCSPFRQ %f\n", Frq));
	
Error:
	return error;
}

/****************************************************************************/
/*                SOURCE SWEEP STOP FREQUENCY GET FUNCTION      	 		*/
/*					          												*/
/*	Returns the source sweep stop frequency.								*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSourceSweepStopFrequency(ViSession vi, 
													  ViReal64 *Frq)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "SRCSPFRQ?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "SRCSPFRQ %f", Frq));
	
Error:
	return error;
}

/****************************************************************************/
/*                  SOURCE SWEEP STOP POWER SET FUNCTION  	    	 		*/
/*					          												*/
/*	Sets the source sweep stop power.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSourceSweepStopPower(ViSession vi, ViReal64 Pow)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "SRCSPPWR %f\n", Pow));
	
Error:
	return error;
}

/****************************************************************************/
/*                  SOURCE SWEEP STOP POWER GET FUNCTION        	 		*/
/*					          												*/
/*	Returns the source sweep stop power.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSourceSweepStopPower(ViSession vi, ViReal64 *Pow)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "SRCSPPWR?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "SRCSPPWR %f", Pow));

Error:
	return error;
}

/****************************************************************************/
/*                             START FUNCTION                   	 		*/
/*					          												*/
/*	Startup self test.														*/
/*																			*/
/****************************************************************************/
									   //enum StartupSelfTest   
ViStatus _VI_FUNC ML2430_Start(ViSession vi, ViInt32 *Sttus)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 TmpInt, Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "START\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%d", &TmpInt));
	*Sttus = TmpInt+1;
	
Error:
	return error;
}

/****************************************************************************/
/*                          STATUS MESSAGE FUNCTION               	 		*/
/*					          												*/
/*	Returns the GPIB status, the status of both channels, and the status	*/
/*  of both sensors.														*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_StatusMessage(ViSession vi, ViInt32 *OpMode,
									   struct GPIBStatus *GPIBSt,
									   struct ChannelStatus *ChanSt1,
									   struct ChannelStatus *ChanSt2,
									   struct SensorStatus *SnsrSt1,
									   struct SensorStatus *SnsrSt2)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpChr[5], Buf[200];
	ViInt32 Bufsize = sizeof(Buf)-1;

	checkErr(viPrintf(vi, "STATUS\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	TmpChr[0] = Buf[0];
	TmpChr[1] = 0x00;
	*OpMode = atol(TmpChr);

	TmpChr[0] = Buf[1];
	ChanSt1->InptCnfg = atol(TmpChr);
	TmpChr[0] = Buf[2];
	ChanSt2->InptCnfg = atol(TmpChr);
	TmpChr[0] = Buf[3];
	ChanSt1->Unts	  = atol(TmpChr);
	TmpChr[0] = Buf[4];
	ChanSt2->Unts	  = atol(TmpChr);
	TmpChr[0] = Buf[5];
	ChanSt1->RelSt	  = atol(TmpChr);
	TmpChr[0] = Buf[6];
	ChanSt2->RelSt	  = atol(TmpChr);
	TmpChr[0] = Buf[7];
	ChanSt1->LLSt	  = atol(TmpChr);
	TmpChr[0] = Buf[8];
	ChanSt1->HLSt	  = atol(TmpChr);
	TmpChr[0] = Buf[9];
	ChanSt2->LLSt	  = atol(TmpChr);
	TmpChr[0] = Buf[10];
	ChanSt2->HLSt	  = atol(TmpChr);
	
	TmpChr[0] = Buf[11];
	SnsrSt1->MeasMde  = atol(TmpChr);
	TmpChr[0] = Buf[12];
	SnsrSt2->MeasMde  = atol(TmpChr);
	TmpChr[2] = 0x00;
	TmpChr[0] = Buf[13];
	TmpChr[1] = Buf[14];
	SnsrSt1->RngeHld  = atol(TmpChr);
	TmpChr[0] = Buf[15];
	TmpChr[1] = Buf[16];
	SnsrSt2->RngeHld  = atol(TmpChr);
	TmpChr[1] = 0x00;
	TmpChr[0] = Buf[17];
	SnsrSt1->AvgMde   = atol(TmpChr);
	TmpChr[0] = Buf[18];
	SnsrSt2->AvgMde   = atol(TmpChr);
	TmpChr[4] = 0x00;
	TmpChr[0] = Buf[19];
	TmpChr[1] = Buf[20];
	TmpChr[2] = Buf[21];
	TmpChr[3] = Buf[22];
	SnsrSt1->AvgNum  = atol(TmpChr);
	TmpChr[0] = Buf[23];
	TmpChr[1] = Buf[24];
	TmpChr[2] = Buf[25];
	TmpChr[3] = Buf[26];
	SnsrSt2->AvgNum  = atol(TmpChr);
	TmpChr[1] = 0x00;
	TmpChr[0] = Buf[27];
	SnsrSt1->LLAvg	  = atol(TmpChr);
	TmpChr[0] = Buf[28];
	SnsrSt2->LLAvg	  = atol(TmpChr);
	TmpChr[0] = Buf[29];
	SnsrSt1->ZerSt	  = atol(TmpChr);
	TmpChr[0] = Buf[30];
	SnsrSt2->ZerSt	  = atol(TmpChr);

	TmpChr[0] = Buf[31];
	GPIBSt->TrgMde	  = atol(TmpChr);
	TmpChr[0] = Buf[32];
	GPIBSt->GrpTrgMde = atol(TmpChr);
	TmpChr[0] = Buf[33];
	GPIBSt->CalSt	  = atol(TmpChr);
	TmpChr[0] = Buf[34];
	GPIBSt->DspComSt  = atol(TmpChr);
	TmpChr[0] = Buf[35];
	GPIBSt->FastSt	  = atol(TmpChr);
	
Error:
	return error;
}

/****************************************************************************/
/*                   SWEEP AVERAGING POINTS SET FUNCTION  	    	 		*/
/*					          												*/
/*	Sets the sweep averaging points.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSweepAveragingPoints(ViSession vi, ViInt32 Snsr,
												 ViInt16 Pts)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "GRSWP %s,%d\n", TmpStr[Snsr], Pts));
	
Error:
	return error;
}

/****************************************************************************/
/*                   SWEEP AVERAGING POINTS GET FUNCTION        	 		*/
/*					          												*/
/*	Returns the sweep averaging points.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSweepAveragingPoints(ViSession vi, ViInt32 ISnsr,
												 ViInt32 *OSnsr, ViInt32 *Pts)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[2][2] = {"A","B"};
	ViChar TmpChr[3], Tmp[20], Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "GRSWP? %s\n", TmpStr[ISnsr]));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "%s[w*t32q] %s[w*t44q],%d", 19, Tmp, 2, TmpChr, Pts));
	*OSnsr = SensorA;
	if(TmpChr[0]=='B')
		*OSnsr = SensorB;
	
Error:
	return error;
}

/****************************************************************************/
/*                      SWEEP AVERAGING RESET FUNCTION  	    	 		*/
/*					          												*/
/*	Resets the sweep averaging.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ResetSweepAveraging(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "GRSWR\n"));
	
Error:
	return error;
}

/****************************************************************************/
/*                   SWEEP AVERAGING STATE SET FUNCTION  	    	 		*/
/*					          												*/
/*	Sets the sweep averaging state.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetSweepAveragingState(ViSession vi, ViInt32 SwpAvg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "GRSWS %s;\n", Matrix[SwpAvg]));
	
Error:
	return error;
}

/****************************************************************************/
/*                   SWEEP AVERAGING STATE GET FUNCTION    	    	 		*/
/*					          												*/
/*	Returns the sweep averaging state.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetSweepAveragingState(ViSession vi, ViInt32 *SwpAvg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "GRSWS?\n"));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*SwpAvg = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*SwpAvg = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*                         TEXT DISPLAY SET FUNCTION    	    	 		*/
/*					          												*/
/*	Sets the text display on or off.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTextDisplay(ViSession vi, ViInt32 Txt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "TEXTS %s\n", Matrix[Txt]));
	
Error:
	return error;
}

/****************************************************************************/
/*                         TEXT DISPLAY GET FUNCTION    	    	 		*/
/*					          												*/
/*	Returns whether the text display is on or off.							*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTextDisplay(ViSession vi, ViInt32 *Txt)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "TEXTS?\n"));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Txt = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Txt = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*                             TEXT SET FUNCTION    	        	 		*/
/*					          												*/
/*	Sets the text.															*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetText(ViSession vi, ViChar Txt[])
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "TEXT %s\n", Txt));
	
Error:
	return error;
}

/****************************************************************************/
/*                             TEXT GET FUNCTION        	    	 		*/
/*					          												*/
/*	Returns the text.														*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetText(ViSession vi, ViChar Txt[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 Bufsize = sizeof(Buf)-1;
	
	checkErr(viPrintf(vi, "TEXT?\n"));
	checkErr(viRead(vi, Buf, Bufsize, &Bufsize));
	Buf[Bufsize]=0x00;

	checkErr(Scan(Buf, "TEXT %s", Txt));
	
Error:
	return error;
}

/****************************************************************************/
/*                       TRIGGER A READING FUNCTION        	    	 		*/
/*					          												*/
/*	Triggers a single reading.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_TriggerAReading(ViSession vi, ViInt32 Trg, 
										 ViInt32 Chan, ViReal64 *RdRslt,
										 ViReal64 PflRslt[])
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[10000], Buf2[10000], *NewBuf;
	ViInt32 BufSize = sizeof(Buf)-1, i, TmpInt, TmpInt2;
	ViChar Matrix[2][3] = {"TR1","TR2"};
	checkErr(viPrintf(vi, "%s %d\n", Matrix[(Trg-1)], (Chan+1)));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	if(strstr(Buf, "#")==NULL)
	{
		Scan(Buf, "%.;%f", RdRslt);
		for(i=0; i<200; i++)
			PflRslt[i] = 0.00;
	}
	else
	{
		*RdRslt = 0.00;
		Scan(Buf, "#%s", Buf2);
		TmpInt = (ViInt32)Buf2[1];
		NewBuf = malloc(sizeof(char)*(TmpInt+1));
		strncpy(NewBuf, Buf2+2, TmpInt);
		NewBuf[TmpInt] = 0x00;
		TmpInt2 = atol(NewBuf);
		NewBuf = malloc(sizeof(char)*(TmpInt2+1));
		strncpy(NewBuf, Buf2+2+TmpInt, TmpInt2);
		NewBuf[TmpInt2] = 0x00;
		for(i=0; i<(TmpInt2/4); i++)
			PflRslt[i] = ((ViInt8)NewBuf[i*4] + ((ViInt8)NewBuf[(i*4)+1]*256) +
						 ((ViInt8)NewBuf[(i*4)+2]*65536) + 
						 ((ViInt8)NewBuf[(i*4)+3]*16777216))/1024;
	}
	
Error:
	return error;
}

/****************************************************************************/
/*                        TRIGGER ARMING SET FUNCTION           	 		*/
/*					          												*/
/*	Sets the trigger arming on or off.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerArming(ViSession vi, ViInt32 Chan, 
										  ViInt32 Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	ViChar TrgMatrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "TRGARM %s,%s\n", ChanMatrix[Chan], TrgMatrix[Trg]));
	
Error:
	return error;
}

/****************************************************************************/
/*                        TRIGGER ARMING GET FUNCTION     	    	 		*/
/*					          												*/
/*	Returns whether the trigger arming is on or off.						*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerArming(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGARM? %s\n", ChanMatrix[IChan]));
	viRead(vi, Buf, BufSize, &BufSize);
	if (BufSize == 0)
	  checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGARM 1&2")==NULL)
		if(strstr(Buf, "TRGARM 2")==NULL)
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	*Trg = Switch_On;
	if(strstr(Buf, "ON")==NULL)
		*Trg = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*                         TRIGGER DELAY SET FUNCTION           	 		*/
/*					          												*/
/*	Sets the trigger delay.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerDelay(ViSession vi, ViInt32 Chan, 
										 ViReal64 Dly)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGDLY %s,%f\n", ChanMatrix[Chan], Dly));
	
Error:
	return error;
}

/****************************************************************************/
/*                         TRIGGER DELAY GET FUNCTION     	    	 		*/
/*					          												*/
/*	Returns the trigger delay.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerDelay(ViSession vi, ViInt32 IChan, 
										 ViInt32 *OChan, ViReal64 *Dly)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100], Tmp[10];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	
	checkErr(viPrintf(vi, "TRGDLY? %s\n", ChanMatrix[IChan]));
	viRead(vi, Buf, BufSize, &BufSize);
	if (BufSize == 0)
	  checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGDLY 1&2")==NULL)	   
		if(strstr(Buf, "TRGDLY 2")==NULL)  
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	Scan(Buf, "TRGDLY %s[w*t44q],%f", 9, Tmp, Dly);
	
Error:
	return error;
}

/****************************************************************************/
/*                     TRIGGER EXTERNAL EDGE SET FUNCTION          	 		*/
/*					          												*/
/*	Sets the trigger external edge.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerExternalEdge(ViSession vi, ViInt32 Chan, 
										 		ViInt32 Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	ViChar TrgMatrix[2][5] = {"RISE","FALL"};
	checkErr(viPrintf(vi, "TRGXTTL %s,%s\n", ChanMatrix[Chan], TrgMatrix[Trg]));
	
Error:
	return error;
}

/****************************************************************************/
/*                     TRIGGER EXTERNAL EDGE GET FUNCTION       	 		*/
/*					          												*/
/*	Returns the trigger external edge.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerExternalEdge(ViSession vi, ViInt32 IChan, 
										 		ViInt32 *OChan, ViInt32 *Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGXTTL? %s\n", ChanMatrix[IChan]));
	viRead(vi, Buf, BufSize, &BufSize);
	if (BufSize == 0)
	  checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGXTTL 1&2")==NULL)
		if(strstr(Buf, "TRGXTTL 2")==NULL)
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	*Trg = Profile_Rise;
	if(strstr(Buf, "RISE")==NULL)
		*Trg = Profile_Fall;
	
Error:
	return error;
}

/****************************************************************************/
/*                      TRIGGER GATE WIDTH SET FUNCTION          	 		*/
/*					          												*/
/*	Sets the trigger gate width.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerGateWidth(ViSession vi, ViInt32 Chan, 
										 	 ViReal64 Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGGW %s,%.7f\n", ChanMatrix[Chan], Trg));
	
Error:
	return error;
}

/****************************************************************************/
/*                      TRIGGER GATE WIDTH GET FUNCTION         	 		*/
/*					          												*/
/*	Returns the trigger gate width.											*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerGateWidth(ViSession vi, ViInt32 IChan, 
										 	 ViInt32 *OChan, ViReal64 *Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100], Tmp[10];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	
	checkErr(viPrintf(vi, "TRGGW? %s\n", ChanMatrix[IChan]));
	viRead(vi, Buf, BufSize, &BufSize);
	if (BufSize == 0)
	  checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGGW 1&2")==NULL)		
		if(strstr(Buf, "TRGGW 2")==NULL)	
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	Scan(Buf, "TRGGW %s[w*t44q],%f", 9, Tmp, Trg);
	
Error:
	return error;
}

/****************************************************************************/
/*                        TRIGGER LEVEL SET FUNCTION            	 		*/
/*					          												*/
/*	Sets the trigger level.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerLevel(ViSession vi, ViInt32 Chan, 
										 ViReal64 Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGLVL %s,%f\n", ChanMatrix[Chan], Trg));
	
Error:
	return error;
}

/****************************************************************************/
/*                        TRIGGER LEVEL GET FUNCTION            	 		*/
/*					          												*/
/*	Returns the trigger level.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerLevel(ViSession vi, ViInt32 IChan, 
										 ViInt32 *OChan, ViReal64 *Trg)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100], Tmp[20];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	
	checkErr(viPrintf(vi, "TRGLVL? %s\n", ChanMatrix[IChan]));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGLVL 1&2")==NULL)		
		if(strstr(Buf, "TRGLVL 2")==NULL)	
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	Scan(Buf, "TRGLVL %s[w*t44q],%f", 19, Tmp, Trg);
	
Error:
	return error;
}

/****************************************************************************/
/*                       TRIGGER LINKING SET FUNCTION            	 		*/
/*					          												*/
/*	Sets the trigger linking on or off.										*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerLinking(ViSession vi, ViInt32 Lnk)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][4] = {"OFF","ON"};
	checkErr(viPrintf(vi, "LINK %s\n", Matrix[Lnk]));
	
Error:
	return error;
}

/****************************************************************************/
/*                       TRIGGER LINKING GET FUNCTION            	 		*/
/*					          												*/
/*	Returns whether the trigger linking is on or off.						*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerLinking(ViSession vi, ViInt32 *Lnk)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "LINK?\n"));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Lnk = Switch_On;
	if(strstr(Buf, "ON")==NULL)		
		*Lnk = Switch_Off;
	
Error:
	return error;
}

/****************************************************************************/
/*                         TRIGGER MODE SET FUNCTION            	 		*/
/*					          												*/
/*	Sets the trigger mode.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerMode(ViSession vi, ViInt32 Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Matrix[2][5] = {"IND","COMB"};
	checkErr(viPrintf(vi, "TRGMODE %s\n", Matrix[Mode]));
	
Error:
	return error;
}

/****************************************************************************/
/*                         TRIGGER MODE GET FUNCTION            	 		*/
/*					          												*/
/*	Returns the trigger mode.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerMode(ViSession vi, ViInt32 *Mode)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	checkErr(viPrintf(vi, "TRGMODE?\n"));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*Mode = Mode_Individual;
	if(strstr(Buf, "IND")==NULL)		
		*Mode = Mode_Combined;
	
Error:
	return error;
}

/****************************************************************************/
/*                        TRIGGER SOURCE SET FUNCTION            	 		*/
/*					          												*/
/*	Sets the trigger source.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerSource(ViSession vi, ViInt32 Chan, 
										  ViInt32 Src)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	ViChar SrcMatrix[5][7] = {"INTA","INTB","EXTTTL","MANUAL","CONT"};
	checkErr(viPrintf(vi, "TRGSRC %s,%s;\n", ChanMatrix[Chan], SrcMatrix[Src]));
	
Error:
	return error;
}

/****************************************************************************/
/*                        TRIGGER SOURCE GET FUNCTION            	 		*/
/*					          												*/
/*	Returns the trigger source.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerSource(ViSession vi, ViInt32 IChan, 
										  ViInt32 *OChan, ViInt32 *Src)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGSRC? %s\n", ChanMatrix[IChan]));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGSRC 1&2")==NULL)		
		if(strstr(Buf, "TRGSRC 2")==NULL)	
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	*Src = Profile_INTA;
	if(strstr(Buf, "INTA\n")==NULL)		
		if(strstr(Buf, "INTB\n")==NULL)	
			if(strstr(Buf, "CONT\n")==NULL)
				if(strstr(Buf, "MANUAL\n")==NULL)
					*Src = Profile_EXTTTL;
				else
					*Src = Profile_MANUAL;
			else
				*Src = Profile_CONT;
		else
			*Src = Profile_INTB;
	
Error:
	return error;
}

/****************************************************************************/
/*                         TRIGGER TYPE SET FUNCTION            	 		*/
/*					          												*/
/*	Sets the trigger type.													*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_SetTriggerType(ViSession vi, ViInt32 Chan, ViInt32 Typ)
{
	ViStatus error = VI_SUCCESS;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	ViChar TypMatrix[2][5] = {"RISE","FALL"};
	checkErr(viPrintf(vi, "TRGTYP %s,%s;\n", ChanMatrix[Chan], TypMatrix[Typ]));
	
Error:
	return error;
}

/****************************************************************************/
/*                         TRIGGER TYPE GET FUNCTION            	 		*/
/*					          												*/
/*	Returns the trigger type.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_GetTriggerType(ViSession vi, ViInt32 IChan, 
										ViInt32 *OChan, ViInt32 *Typ)
{
	ViStatus error = VI_SUCCESS;
	ViChar Buf[100];
	ViInt32 BufSize = sizeof(Buf)-1;
	ViChar ChanMatrix[3][4] = {"1","2","1&2"};
	checkErr(viPrintf(vi, "TRGTYP? %s\n", ChanMatrix[IChan]));
	checkErr(viRead(vi, Buf, BufSize, &BufSize));
	Buf[BufSize]=0x00;

	*OChan = Channel_1_2;
	if(strstr(Buf, "TRGTYP 1&2")==NULL)		
		if(strstr(Buf, "TRGTYP 2")==NULL)	
			*OChan = Channel_1;
		else
			*OChan = Channel_2;
	*Typ = Profile_Rise;
	if(strstr(Buf, "RISE")==NULL)
		*Typ = Profile_Fall;
	
Error:
	return error;
}

/****************************************************************************/
/*                        ZERO ANALOG INPUT FUNCTION            	 		*/
/*					          												*/
/*	Zeroes the analog input.												*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ZeroAnalogInput(ViSession vi)
{
	ViStatus error = VI_SUCCESS;
	checkErr(viPrintf(vi, "VZERO\n"));
	
Error:
	return error;
}

/****************************************************************************/
/*                            ZERO SENSOR FUNCTION              	 		*/
/*					          												*/
/*	Zeroes out noise from selected sensor.									*/
/*																			*/
/****************************************************************************/
									      
ViStatus _VI_FUNC ML2430_ZeroSensor(ViSession vi, ViInt32 Snsr)
{
	ViStatus error = VI_SUCCESS;
	ViChar TmpStr[2][2] = {"A","B"};
	checkErr(viPrintf(vi, "ZERO %s\n", TmpStr[Snsr]));
	
Error:
	return error;
}


/****************************************************************************/
/*					          												*/
/*			       	       		UTILITY FUNCTIONS		           	  		*/
/*					          												*/
/****************************************************************************/

/****************************************************************************/
/*			       	           FLOAT TO FOUR BYTES               	  		*/
/*					          												*/
/*	Turns a float to four bytes.		                 					*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_FloatToFourBytes(ViReal64 Flt, ViChar Bytes[4])
{
	ViStatus error = VI_SUCCESS;
	ViInt32 i;
	Data32 TempData;

	TempData.FVal = Flt;
	for(i=0; i<4; i++)
		Bytes[i] = TempData.CVal[i];

Error:
	return error;
}

/****************************************************************************/
/*			       	           FOUR BYTES TO FLOAT               	  		*/
/*					          												*/
/*	Turns four bytes to a float.        									*/
/*																			*/
/****************************************************************************/

ViStatus _VI_FUNC ML2430_FourBytesToFloat(ViChar Bytes[4], ViReal64 *Flt)
{
	ViStatus error = VI_SUCCESS;
	ViInt32 i;
	Data32 TempData;
	for(i=0; i<4; i++)
		TempData.CVal[i] = Bytes[i];
	*Flt = TempData.FVal;

Error:
	return error;
}
