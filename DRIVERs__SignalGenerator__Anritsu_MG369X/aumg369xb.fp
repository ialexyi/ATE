s��        ��   \ �  9�   �   6����                               aumg369xb                       Anritsu MG369XB Synthesizer                 _VI_FUNC                                                     �  ViConstString *[]     � ��ViConstString ***     � ��ViConstString **     � ��ViConstString *     �  ViConstString []     � ��ViConstString     
�  	ViByte *[]     
� 
��ViByte ***     	� 	��ViByte **     � ��ViByte *     	�  ViByte []     � ��ViByte     
�  	ViChar *[]     
� 
��ViChar ***     	� 	��ViChar **     � ��ViChar *     	�  ViChar []     � ��ViChar     � 
 ViReal64 *[]      ��ViReal64 ***     ~ ��ViReal64 **     
} 
��ViReal64 *     | 	 
ViReal64 []     { ��ViReal64     z 	 
ViInt32 *[]     y ��ViInt32 ***     
x 
��ViInt32 **     	w 	��ViInt32 *     
v  	ViInt32 []     u ��ViInt32     t  ViSession *[]     s ��ViSession ***     r ��ViSession **     q ��ViSession *     p 
 ViSession []     	o 	��ViSession     n  ViBoolean *[]     m ��ViBoolean ***     l ��ViBoolean **     k ��ViBoolean *     j 
 ViBoolean []     	i 	��ViBoolean     
h  	ViRsrc *[]     
g 
��ViRsrc ***     	f 	��ViRsrc **     e ��ViRsrc *     	d  ViRsrc []     c ��ViRsrc     b 
 ViStatus *[]     a ��ViStatus ***     ` ��ViStatus **     
_ 
��ViStatus *     ^ 	 
ViStatus []     ] ��ViStatus   l    This instrument driver provides programming support for the Anritsu MG369XB Synthesized Signal Generators.     �    This class of functions configures the instrument parameters, including frequency, level and modulation.

SubClasses:
(1) RF
(2) Frequency
(3) Level
(4) Modulation
(5) Measurement
(6) System     V    This class contains functions that turn on the output and configure the RF settings.     O    This class contains functions that configure the frequency of the instrument.     F    This class contains functions that configure the output power level.     i    This class contains functions that configure the modulation, including AM, FM, PM and Pulse Modulation.     [    This class contains functions that configure the AM, FM and Power Meter measurement mode.     C    This class contains functions that configure the system settings.     N    This class contains functions that set the current status of the instrument.    V    This class contains functions and sub-classes that control common instrument operations. These functions include many of functions that VXIplug&play require, such as reset, self-test, revision query, error query, and error message. This class also contains functions that manipulate the instrument setup data and the user level calibration.     X    This class contains functions that save, recall, write and read instrument setup data.     Q    This class contains functions that write and read user level calibration table.    �    This function performs the following initialization actions:

- Opens a session to the Default Resource Manager resource and a session to the specified device using the interface and address specified in the Resource_Name control.

- Performs an identification query on the Instrument.

- Resets the instrument to a known state.

- Sends initialization commands to the instrument that set any necessary programmatic variables such as Headers Off, Short Command form, and Data Transfer Binary to the state necessary for the operation of the instrument driver.

- Returns an Instrument Handle which is used to differentiate between different sessions of this instrument driver.

- Each time this function is invoked an Unique Session is opened.  It is possible to have more than one session open for the same resource.

Notes:

(1) If this instrument does not support an ID Query, and the ID Query control is set to "Do Query" then this function should return the Warning Code 0x3FFC0101 - VI_WARN_NSUP_ID_QUERY.
   
(2) If this instrument does not support a Reset, and the Reset control is set to "Reset Device" then this function should return the Warning Code 0x3FFC0102 - VI_WARN_NSUP_RESET.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;     y    This control specifies the interface and address of the device that is to be initialized (Instrument Descriptor). The exact grammar to be used in this control is shown in the note below. 

Default Value:  "GPIB0::5::INSTR"

Notes:

(1) Based on the Instrument Descriptor, this operation establishes a communication session with a device.  The grammar for the Instrument Descriptor is shown below.  Optional parameters are shown in square brackets ([]).

Interface   Grammar
------------------------------------------------------
GPIB        GPIB[board]::primary address[::secondary address]
            [::INSTR]
            
The GPIB keyword is used with GPIB instruments.

The default value for optional parameters are shown below.

Optional Parameter          Default Value
-----------------------------------------
board                       0
secondary address           none - 31
    #    This control specifies if an ID Query is sent to the instrument during the initialization procedure.

Valid Values:
VI_FALSE (0) - Skip Query
VI_TRUE  (1) - Do Query (Default Value)

Notes:
   
(1) Under normal circumstances the ID Query ensures that the instrument initialized is the type supported by this driver. However circumstances may arise where it is undesirable to send an ID Query to the instrument.  In those cases; set this control to "Skip Query" and this function will initialize the selected interface, without doing an ID Query.    H    This control specifies if the instrument is to be reset to its power-on settings during the initialization procedure.

Valid Values:
VI_FALSE (0) - Don't Reset
VI_TRUE  (1) - Reset Device (Default Value)

Notes:

(1) If you do not want the instrument reset. Set this control to "Don't Reset" while initializing the instrument.    6    This control returns an Instrument Handle that is used in all subsequent function calls to differentiate between different sessions of this instrument driver.

Notes:

(1) Each time this function is invoked a Unique Session is opened.  It is possible to have more than one session open for the same resource.    ���]  �    Status                            � -   c  �    Resource Name                     Q - i       ID Query                          | -� i       Reset Device                      �  o  �    Instrument Handle                  	            "GPIB0::5::INSTR"   On VI_TRUE Off VI_FALSE   On VI_TRUE Off VI_FALSE    	            4    This function turns on or turns off the RF output.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     d    Specifies whether to turn on the RF output.

Valid Values:

VI_FALSE - Off
VI_TRUE  - On (Default)     ����]  �    Status                            'p   o  �    Instrument Handle                 ( - � i       Output Enabled                     	                Enable VI_TRUE Disable VI_FALSE   m    This function configures the RF parameters, including the RF state during retrace, the RF state during frequency switching, the ramp rest point for sweeps that are triggered by a single trigger or external trigger, the RF state at reset, the frequency scaling reference multiplier and the attenuation when RF is switched off.                                          �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     d    Selects RF to be on or off during retrace.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Selects RF to be on or off during frequency switching in CW, step sweep, and list sweep modes.

Valid Values:

VI_FALSE - Off
VI_TRUE  - On (Default)     q    Sets 0V or 10V of ramp rest.

Valid Values:

AUMG369XB_RAMP_REST_0  - 0V
AUMG369XB_RAMP_REST_10 - 10V (Default)     ^    Selects RF to be on or off at reset.

Valid Values:

VI_FALSE - On (Default)
VI_TRUE  - Off      �    Specifies the frequency scaling reference multiplier parameter. 

Valid Range: 0.1 to 14

Default Value: 1.0

Unit: TMS

Notes:

This setting affects all entered and displayed frequencies, but does not affect the output of the instrument.     s    Sets 0dB or 40dB of attenuation when RF is switched off.

Valid Values:

VI_FALSE - 0dB
VI_TRUE  - 40dB (Default)    *����]  �    Status                            1�   o  �    Instrument Handle                 2 - P i       Retrace RF                        2� - � i       Delta - F RF                      3* -� u �    Ramp Rest                         3� � P i       Reset State                       4	 � � {  �    Frequency Scaling                 5 �� i       Terminate RF                       	                On VI_TRUE Off VI_FALSE   On VI_TRUE Off VI_FALSE              20 AUMG369XB_RAMP_REST_0 10 AUMG369XB_RAMP_REST_10    On VI_TRUE Off VI_FALSE    1.0   On VI_TRUE Off VI_FALSE    �    This function sets the frequency list, including the F0 to F9 and M0 to M9 parameters.                                                                   �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    �    Configures the F0 to F9 and M0 to M9 parameters. 

Valid Range: Frequency range of the MG369XB model

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz

Notes:

(1) The array must contain 20 ViReal64 elements.    8����]  �    Status                            ?E   o  �    Instrument Handle                 ?� - � |  �    Frequency List                     	                    ,    This function sets the selected frequency.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    y    Specifies the frequency parameter to be set.

Valid Values:

AUMG369XB_FREQUENCY_F0 - F0
AUMG369XB_FREQUENCY_F1 - F1 (Default)
AUMG369XB_FREQUENCY_F2 - F2
AUMG369XB_FREQUENCY_F3 - F3
AUMG369XB_FREQUENCY_F4 - F4
AUMG369XB_FREQUENCY_F5 - F5
AUMG369XB_FREQUENCY_F6 - F6 
AUMG369XB_FREQUENCY_F7 - F7
AUMG369XB_FREQUENCY_F8 - F8
AUMG369XB_FREQUENCY_F9 - F9
AUMG369XB_FREQUENCY_M0 - M0
AUMG369XB_FREQUENCY_M1 - M1
AUMG369XB_FREQUENCY_M2 - M2
AUMG369XB_FREQUENCY_M3 - M3
AUMG369XB_FREQUENCY_M4 - M4
AUMG369XB_FREQUENCY_M5 - M5
AUMG369XB_FREQUENCY_M6 - M6
AUMG369XB_FREQUENCY_M7 - M7
AUMG369XB_FREQUENCY_M8 - M8
AUMG369XB_FREQUENCY_M9 - M9    {    Specifies the frequency value.

Valid Range: Frequency range of the MG369XB model

Default Value: 2.2E9

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz    E����]  �    Status                            LC   o  �    Instrument Handle                 L� -  u �    Selected Frequency                Oc - � {  �    Frequency (Hz)                     	                         F0 AUMG369XB_FREQUENCY_F0 F1 AUMG369XB_FREQUENCY_F1 F2 AUMG369XB_FREQUENCY_F2 F3 AUMG369XB_FREQUENCY_F3 F4 AUMG369XB_FREQUENCY_F4 F5 AUMG369XB_FREQUENCY_F5 F6 AUMG369XB_FREQUENCY_F6 F7 AUMG369XB_FREQUENCY_F7 F8 AUMG369XB_FREQUENCY_F8 F9 AUMG369XB_FREQUENCY_F9 M0 AUMG369XB_FREQUENCY_M0 M1 AUMG369XB_FREQUENCY_M1 M2 AUMG369XB_FREQUENCY_M2 M3 AUMG369XB_FREQUENCY_M3 M4 AUMG369XB_FREQUENCY_M4 M5 AUMG369XB_FREQUENCY_M5 M6 AUMG369XB_FREQUENCY_M6 M7 AUMG369XB_FREQUENCY_M7 M8 AUMG369XB_FREQUENCY_M8 M9 AUMG369XB_FREQUENCY_M9    2.2E9    )    This function sets the delta frequency.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    �    Specifies the delta frequency parameter.

Valid Range: Frequency range of the MG369XB model

Default Value: 1.0E9

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz    W���]  �    Status                            ]�   o  �    Instrument Handle                 ^x - � {  �    Delta Frequency (Hz)               	                1.0E9    �    This function places the MG369XB in CW mode and configures the CW requency parameters, including the current frequency, the phase offset and the CW ramp.
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    e    Selects a CW frequency.

Valid Values:

AUMG369XB_FREQUENCY_F0 - F0
AUMG369XB_FREQUENCY_F1 - F1 (Default)
AUMG369XB_FREQUENCY_F2 - F2
AUMG369XB_FREQUENCY_F3 - F3
AUMG369XB_FREQUENCY_F4 - F4
AUMG369XB_FREQUENCY_F5 - F5
AUMG369XB_FREQUENCY_F6 - F6 
AUMG369XB_FREQUENCY_F7 - F7
AUMG369XB_FREQUENCY_F8 - F8
AUMG369XB_FREQUENCY_F9 - F9
AUMG369XB_FREQUENCY_M0 - M0
AUMG369XB_FREQUENCY_M1 - M1
AUMG369XB_FREQUENCY_M2 - M2
AUMG369XB_FREQUENCY_M3 - M3
AUMG369XB_FREQUENCY_M4 - M4
AUMG369XB_FREQUENCY_M5 - M5
AUMG369XB_FREQUENCY_M6 - M6
AUMG369XB_FREQUENCY_M7 - M7
AUMG369XB_FREQUENCY_M8 - M8
AUMG369XB_FREQUENCY_M9 - M9
    *    Turns on or turns off the CW ramp.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On 

Notes:

Turning on the CW ramp produces a repetitive 0V to 10V ramp output to the rear panel HORIZ OUT connector and pin 1 of the AUX I/O connector. The CW ramp is used to drive a scalar analyzer display.     a    Turns on or turns off the phase offset.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      c    Configures the phase offset parameter.

Valid Range: -360 to - +360

Default Value: 0.0

Unit: DG    da���]  �    Status                            k   o  �    Instrument Handle                 k� -  u �    Selected Frequency                n+ � F i       CW Ramp Enabled                   o] - � i       Phase Offset Enabled              o� -� {  �    Phase Offset (Degree)              	                         F0 AUMG369XB_FREQUENCY_F0 F1 AUMG369XB_FREQUENCY_F1 F2 AUMG369XB_FREQUENCY_F2 F3 AUMG369XB_FREQUENCY_F3 F4 AUMG369XB_FREQUENCY_F4 F5 AUMG369XB_FREQUENCY_F5 F6 AUMG369XB_FREQUENCY_F6 F7 AUMG369XB_FREQUENCY_F7 F8 AUMG369XB_FREQUENCY_F8 F9 AUMG369XB_FREQUENCY_F9 M0 AUMG369XB_FREQUENCY_M0 M1 AUMG369XB_FREQUENCY_M1 M2 AUMG369XB_FREQUENCY_M2 M3 AUMG369XB_FREQUENCY_M3 M4 AUMG369XB_FREQUENCY_M4 M5 AUMG369XB_FREQUENCY_M5 M6 AUMG369XB_FREQUENCY_M6 M7 AUMG369XB_FREQUENCY_M7 M8 AUMG369XB_FREQUENCY_M8 M9 AUMG369XB_FREQUENCY_M9     Enable VI_TRUE Disable VI_FALSE     Enable VI_TRUE Disable VI_FALSE    0.0    �    This function configures the fast-frequency-switching parameters, including the table location, the frequency list, and the table pointer.
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     K    Specifies the table location

Valid Range: 000 - 999

Default Value: 000
    �    Specifies the frequency list that be loaded into the table.

Valid Range: Frequency range of the MG369XB model

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz     B    Specifies the size of the frequency list.

Valid Range: 1 - 1000     i    Sets the table pointer to point to the specified location.

Valid Range: 000 - 999

Default Value: 000
    tx���]  �    Status                            {6   o  �    Instrument Handle                 {� -  u  �    Table Location                    |( -� |  �    Frequency List                    �� - � u  �    Number of Frequency               �� �  u  �    Pointer Location                   	                0            0    �    This function sets up the MG369XB for a analog frequency sweep and configures the analog sweep frequency parameters, including the sweep range, the sweep time and the trigger source.                         
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     ]    Sets the time for the step sweep.

Valid Range: 0.03 to 99

Default Value: 0.05

Unit: SEC
        Specifies the trigger source for the analog sweep frequency.

Valid Values:

AUMG369XB_TRIGGER_SOURCE_AUTO     - Auto (Default)
AUMG369XB_TRIGGER_SOURCE_EXTERNAL - External
AUMG369XB_TRIGGER_SOURCE_SINGLE   - Single
AUMG369XB_TRIGGER_SOURCE_MANUAL   - Manual
    �    Specifies the analog sweep range.

Valid Values:

AUMG369XB_SWEEP_RANGE_FULL  - Full
AUMG369XB_SWEEP_RANGE_F1_F2 - F1 - F2 (Default)
AUMG369XB_SWEEP_RANGE_F3_F4 - F3 - F4
AUMG369XB_SWEEP_RANGE_F0_DF - F0 - dF 
AUMG369XB_SWEEP_RANGE_F1_DF - F1 - dF 
AUMG369XB_SWEEP_RANGE_F5_DF - F5 - dF
AUMG369XB_SWEEP_RANGE_F6_DF - F6 - dF

Notes:

The F-delta frequency sweep mode selects a symmetrical frequency sweep around F. The width of the sweep is determined by the Delta Frequency parameter.
    �����]  �    Status                            �e   o  �    Instrument Handle                 � - � {  �    Sweep Time (Sec)                  �i -� u �    Trigger Source                    �v -  u �    Sweep Range                        	                0.05               �Auto AUMG369XB_TRIGGER_SOURCE_AUTO External AUMG369XB_TRIGGER_SOURCE_EXTERNAL Single AUMG369XB_TRIGGER_SOURCE_SINGLE Manual AUMG369XB_TRIGGER_SOURCE_MANUAL              �Full AUMG369XB_SWEEP_RANGE_FULL F1 - F2 AUMG369XB_SWEEP_RANGE_F1_F2 F3 - F4 AUMG369XB_SWEEP_RANGE_F3_F4 F0 - dF AUMG369XB_SWEEP_RANGE_F0_DF F1 - dF AUMG369XB_SWEEP_RANGE_F1_DF F5 - dF AUMG369XB_SWEEP_RANGE_F5_DF F6 - dF AUMG369XB_SWEEP_RANGE_F6_DF       This function sets up the MG369XB for a step frequency sweep and configures the step sweep frequency parameters, including the step sweep range, the sweep mode, the dwell-time-per-step, the number of steps and the trigger source.                                                     
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Specifies the step sweep mode.

Valid Values:

AUMG369XB_STEP_SWEEP_MODE_LOGARITHMIC - Log
AUMG369XB_STEP_SWEEP_MODE_LINEAR      - Linear (Default)    �    Specifies the step sweep range.

Valid Values:

AUMG369XB_SWEEP_RANGE_FULL  - Full
AUMG369XB_SWEEP_RANGE_F1_F2 - F1 - F2 (Default)
AUMG369XB_SWEEP_RANGE_F3_F4 - F3 - F4
AUMG369XB_SWEEP_RANGE_F0_DF - F0 - dF
AUMG369XB_SWEEP_RANGE_F1_DF - F1 - dF
AUMG369XB_SWEEP_RANGE_F5_DF - F5 - dF
AUMG369XB_SWEEP_RANGE_F6_DF - F6 - dF

Notes:

The F-delta frequency sweep mode selects a symmetrical frequency sweep around F. The width of the sweep is determined by the Delta Frequency parameter.
     �    Specifies the trigger source for the step sweep frequency.

Valid Values:

AUMG369XB_TRIGGER_SOURCE_AUTO     - Auto (Default)
AUMG369XB_TRIGGER_SOURCE_EXTERNAL - External
AUMG369XB_TRIGGER_SOURCE_SINGLE   - Single     k    Specifies the number of steps for the step sweep.

Valid Range: 1 to 10000

Default Value: 50

Unit: SPS
     y    Specifies the dwell-time-per-step value for the step sweep.

Valid Range: 0.001 to 99

Default Value: 0.001

Unit: SEC
    �^���]  �    Status                            �   o  �    Instrument Handle                 �� - � u �    Step Sweep Mode                   �X -  u �    Sweep Range                       �D � � u �    Trigger Source                    �# �  u  �    Number of Steps                   �� ,� {  �    Dwell Time (Sec)                   	                          RLog AUMG369XB_STEP_SWEEP_MODE_LOGARITHMIC Linear AUMG369XB_STEP_SWEEP_MODE_LINEAR              �Full AUMG369XB_SWEEP_RANGE_FULL F1 - F2 AUMG369XB_SWEEP_RANGE_F1_F2 F3 - F4 AUMG369XB_SWEEP_RANGE_F3_F4 F0 - dF AUMG369XB_SWEEP_RANGE_F0_DF F1 - dF AUMG369XB_SWEEP_RANGE_F1_DF F5 - dF AUMG369XB_SWEEP_RANGE_F5_DF F6 - dF AUMG369XB_SWEEP_RANGE_F6_DF               uAuto AUMG369XB_TRIGGER_SOURCE_AUTO External AUMG369XB_TRIGGER_SOURCE_EXTERNAL Single AUMG369XB_TRIGGER_SOURCE_SINGLE    50    0.001    6    his function configures the time for the step sweep.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     b    Specifies the time for the step sweep.

Valid Range: 0.03 to 99

Default Value: 0.05

Unit: SEC
    �����]  �    Status                            ��   o  �    Instrument Handle                 �0 - � {  �    Sweep Time (Sec)                   	                0.05    �    This function configures the alternate sweep frequency parameters, including the sweep range and the power level.                      
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    m    Specifies the sweep range for the alternate sweep.

Valid Values:

AUMG369XB_ALTERNATE_SWEEP_RANGE_FULL  - Full
AUMG369XB_ALTERNATE_SWEEP_RANGE_F1_F2 - F1 - F2
AUMG369XB_ALTERNATE_SWEEP_RANGE_F3_F4 - F3 - F4 (Default)
AUMG369XB_ALTERNATE_SWEEP_RANGE_F1_DF - F1 - dF
AUMG369XB_ALTERNATE_SWEEP_RANGE_F5_DF - F5 - dF
AUMG369XB_ALTERNATE_SWEEP_RANGE_F6_DF - F6 - dF

    �    Selects the alternate frequency sweep RF output power level.

Valid Values:

AUMG369XB_POWER_LEVEL_L0 - L0
AUMG369XB_POWER_LEVEL_L1 - L1 (Default)
AUMG369XB_POWER_LEVEL_L2 - L2
AUMG369XB_POWER_LEVEL_L3 - L3
AUMG369XB_POWER_LEVEL_L4 - L4
AUMG369XB_POWER_LEVEL_L5 - L5
AUMG369XB_POWER_LEVEL_L6 - L6
AUMG369XB_POWER_LEVEL_L7 - L7
AUMG369XB_POWER_LEVEL_L8 - L8
AUMG369XB_POWER_LEVEL_L9 - L9

    �����]  �    Status                            ��   o  �    Instrument Handle                 �@ -  u �    Sweep Range                       �� - � u �    Selected Power Level               	                         Full AUMG369XB_ALTERNATE_SWEEP_RANGE_FULL F1 - F2 AUMG369XB_ALTERNATE_SWEEP_RANGE_F1_F2 F3 - F4 AUMG369XB_ALTERNATE_SWEEP_RANGE_F3_F4 F1 - dF AUMG369XB_ALTERNATE_SWEEP_RANGE_F1_DF F5 - dF AUMG369XB_ALTERNATE_SWEEP_RANGE_F5_DF F6 - dF AUMG369XB_ALTERNATE_SWEEP_RANGE_F6_DF           
  L0 AUMG369XB_POWER_LEVEL_L0 L1 AUMG369XB_POWER_LEVEL_L1 L2 AUMG369XB_POWER_LEVEL_L2 L3 AUMG369XB_POWER_LEVEL_L3 L4 AUMG369XB_POWER_LEVEL_L4 L5 AUMG369XB_POWER_LEVEL_L5 L6 AUMG369XB_POWER_LEVEL_L6 L7 AUMG369XB_POWER_LEVEL_L7 L8 AUMG369XB_POWER_LEVEL_L8 L9 AUMG369XB_POWER_LEVEL_L9    =    This function enables or disables the dual step sweep mode.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    R    Selects or deselects dual step sweep mode.

Valid Values:

VI_FALSE - Deselect  (Default)
VI_TRUE  - Select

Notes:

The dual step sweep mode provides for generating synchronized, step sweep outputs from two MG369XBs at a frequency offset. When operating in the dual step sweep mode, the command, TSS, steps the sweep to the next point.    �����]  �    Status                            �p   o  �    Instrument Handle                 � - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    F    This function enables or disables the non-equally spaced step sweep.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     u    Selects or deselects non-equally spaced step sweep.

Valid Values:

VI_FALSE - Deselect (Default)
VI_TRUE  - Select    ���]  �    Status                            �G   o  �    Instrument Handle                 �� - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    �    This function sets up the MG369XB for a manual sweep frequency and configures the manual sweep parameters, including the sweep range and the number of steps.                            �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    �    Specifies the sweep range for the manual sweep.

Valid Values:

AUMG369XB_SWEEP_RANGE_FULL  - Full
AUMG369XB_SWEEP_RANGE_F1_F2 - F1 - F2 (Default)
AUMG369XB_SWEEP_RANGE_F3_F4 - F3 - F4
AUMG369XB_SWEEP_RANGE_F0_DF - F0 - dF
AUMG369XB_SWEEP_RANGE_F1_DF - F1 - dF
AUMG369XB_SWEEP_RANGE_F5_DF - F5 - dF
AUMG369XB_SWEEP_RANGE_F6_DF - F6 - dF

Notes:

The F-delta frequency sweep mode selects a symmetrical frequency sweep around F. The width of the sweep is determined by the Delta Frequency parameter.
     k    Specifies the number of steps for the step sweep.

Valid Range: 1 to 10000

Default Value: 50

Unit: SPS
    �����]  �    Status                            Ҳ   o  �    Instrument Handle                 �Q -  u �    Sweep Range                       �M - � u  �    Number of Steps                    	                          �Full AUMG369XB_SWEEP_RANGE_FULL F1 - F2 AUMG369XB_SWEEP_RANGE_F1_F2 F3 - F4 AUMG369XB_SWEEP_RANGE_F3_F4 F0 - dF AUMG369XB_SWEEP_RANGE_F0_DF F1 - dF AUMG369XB_SWEEP_RANGE_F1_DF F5 - dF AUMG369XB_SWEEP_RANGE_F5_DF F6 - dF AUMG369XB_SWEEP_RANGE_F6_DF    50    �    This function places the MG369XB in list sweep mode and configures the list sweep parameters, including the list number, the start index, the stop index, the dwell time and the trigger source.                          �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     l    Sets the list start index for a list sweep of the current list.

Valid Range: 0 to 1999

Default Value: 0
     m    Sets the list stop index for a list sweep of the current list.

Valid Range: 0 to 1999

Default Value: 1999     k    Sets the dwell-time-per-step of the list sweep.

Valid Range: 0.001 to 99

Default Value: 0.05

Unit: SEC        Specifies the trigger source for the list sweep frequency.

Valid Values:

AUMG369XB_TRIGGER_SOURCE_AUTO     - Auto 
AUMG369XB_TRIGGER_SOURCE_EXTERNAL - External
AUMG369XB_TRIGGER_SOURCE_SINGLE   - Single
AUMG369XB_TRIGGER_SOURCE_MANUAL   - Manual (Default)
     �    Selects which of the four lists will be accessed.

Valid Values:

AUMG369XB_LIST_NUMBER_0 - 0 (Default)
AUMG369XB_LIST_NUMBER_1 - 1
AUMG369XB_LIST_NUMBER_2 - 2
AUMG369XB_LIST_NUMBER_3 - 3
    ؑ���]  �    Status                            �O   o  �    Instrument Handle                 �� - � u  �    Start Index                       �b -� u  �    Stop Index                        �� �  {  �    Dwell Time (Sec)                  �J � � u �    Trigger Source                    �V -  u �    List Number                        	                0    1999    0.05              �Auto AUMG369XB_TRIGGER_SOURCE_AUTO External AUMG369XB_TRIGGER_SOURCE_EXTERNAL Single AUMG369XB_TRIGGER_SOURCE_SINGLE Manual AUMG369XB_TRIGGER_SOURCE_MANUAL               h0 AUMG369XB_LIST_NUMBER_0 1 AUMG369XB_LIST_NUMBER_1 2 AUMG369XB_LIST_NUMBER_2 3 AUMG369XB_LIST_NUMBER_3    �    This function sets the frequencies and power levels of the selected list starting at the list index.                                              
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     V    Sets the list index for the current list.

Valid Range: 0 to 1999

Default Value: 0
    �    Sets list frequencies of the selected list starting at the list index. 

Valid Range: Frequency range of the MG369XB model.

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz     C    Specifies the size of the frequency list.

Valid Range: 1 to 1999     �    Sets the power levels of the selected list starting at the list index. 

Valid Range: Power level range of the MG369XB model.

Unit: DM
     E    Specifies the size of the power level list.

Valid Range: 1 to 1999     �    Selects which of the four lists will be accessed.

Valid Values:

AUMG369XB_LIST_NUMBER_0 - 0 (Default)
AUMG369XB_LIST_NUMBER_1 - 1
AUMG369XB_LIST_NUMBER_2 - 2
AUMG369XB_LIST_NUMBER_3 - 3    �s���]  �    Status                            �1   o  �    Instrument Handle                 �� - � u  �    List Index                        �. �  |  �    Frequency List                    �� -� u  �    Number of Frequency               � �� |  �    Power Level List                  � � � u  �    Number of Power Level             �� -  u �    List Number                        	                0                               h0 AUMG369XB_LIST_NUMBER_0 1 AUMG369XB_LIST_NUMBER_1 2 AUMG369XB_LIST_NUMBER_2 3 AUMG369XB_LIST_NUMBER_3    �    This function turns on or turns off the master-slave mode of operation. If the master-slave mode is turned on, this function configures the slave delta frequency, the slave main power level and the slave alternate sweep power level.                    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    E    Turns on or turns off the master-slave mode of operation.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On 

Notes:

Master-slave operation provides for enabling two MG369XBs that are connected in a master-slave configuration to produce CW and synchronized, swept output signals at a frequency offset. In a master-slave configuration, one instrument (the master) controls the other (the slave) via interface cables between their rear panel AUX I/O and SERIAL I/O connectors. The two units are phase-locked together by connecting them to the same 10 MHz reference time base.    �    Sets the delta-F frequency parameter for the slave MG369XB unit.

Valid Range: Frequency range of the slave MG369XB model.

Default Value: 1.0E9

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz     �    Sets the main power level parameter (L1) of the slave unit.

Valid Range: Power level range of the slave MG369XB model..

Default Value: 0.0

Units: DM (Log), VT (Linear)     �    Sets the alternate sweep power level parameter (L2) of the slave unit.

Valid Range: Power level range of the slave MG369XB model..

Default Value: 0.0

Units: DM (Log), VT (Linear)
    �����]  �    Status                            ��   o  �    Instrument Handle                 �W - F i       Master Slave Enabled             � - � {  �    Slave Delta Frequency (Hz)       P -� {  �    Slave L1                          �  {  �    Slave L2                           	                 Enable VI_TRUE Disable VI_FALSE    1.0E9    0.0    0.0    @    This function configures the frequency list of the slave unit.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    �    Configures the F0 to F9 and M0 to M9 parameters for the slave unit. 

Valid Range: Frequency range of the MG369XB model

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz

Notes:

(1) The array must contain 20 ViReal64 elements.   	����]  �    Status                           R   o  �    Instrument Handle                � - � |  �    Frequency List                     	                    8    This function configures the selected slave frequency.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    y    Specifies the frequency parameter to be set.

Valid Values:

AUMG369XB_FREQUENCY_F0 - F0
AUMG369XB_FREQUENCY_F1 - F1 (Default)
AUMG369XB_FREQUENCY_F2 - F2
AUMG369XB_FREQUENCY_F3 - F3
AUMG369XB_FREQUENCY_F4 - F4
AUMG369XB_FREQUENCY_F5 - F5
AUMG369XB_FREQUENCY_F6 - F6 
AUMG369XB_FREQUENCY_F7 - F7
AUMG369XB_FREQUENCY_F8 - F8
AUMG369XB_FREQUENCY_F9 - F9
AUMG369XB_FREQUENCY_M0 - M0
AUMG369XB_FREQUENCY_M1 - M1
AUMG369XB_FREQUENCY_M2 - M2
AUMG369XB_FREQUENCY_M3 - M3
AUMG369XB_FREQUENCY_M4 - M4
AUMG369XB_FREQUENCY_M5 - M5
AUMG369XB_FREQUENCY_M6 - M6
AUMG369XB_FREQUENCY_M7 - M7
AUMG369XB_FREQUENCY_M8 - M8
AUMG369XB_FREQUENCY_M9 - M9    {    Specifies the frequency value.

Valid Range: Frequency range of the MG369XB model

Default Value: 2.2E9

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz   ����]  �    Status                           o   o  �    Instrument Handle                 -  u �    Selected Frequency                � - � {  �    Frequency (Hz)                     	                         F0 AUMG369XB_FREQUENCY_F0 F1 AUMG369XB_FREQUENCY_F1 F2 AUMG369XB_FREQUENCY_F2 F3 AUMG369XB_FREQUENCY_F3 F4 AUMG369XB_FREQUENCY_F4 F5 AUMG369XB_FREQUENCY_F5 F6 AUMG369XB_FREQUENCY_F6 F7 AUMG369XB_FREQUENCY_F7 F8 AUMG369XB_FREQUENCY_F8 F9 AUMG369XB_FREQUENCY_F9 M0 AUMG369XB_FREQUENCY_M0 M1 AUMG369XB_FREQUENCY_M1 M2 AUMG369XB_FREQUENCY_M2 M3 AUMG369XB_FREQUENCY_M3 M4 AUMG369XB_FREQUENCY_M4 M5 AUMG369XB_FREQUENCY_M5 M6 AUMG369XB_FREQUENCY_M6 M7 AUMG369XB_FREQUENCY_M7 M8 AUMG369XB_FREQUENCY_M8 M9 AUMG369XB_FREQUENCY_M9    2.2E9    }    This function configures the frequency offset for the Slave instrument in the VNA mode.                                   
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Sets the frequency offset parameter for the slave unit.

Valid Range: Frequency range of the MG369XB model.

Default Value: 1.0E9

Unit: HZ   (����]  �    Status                           /Y   o  �    Instrument Handle                /� - � {  �    VNA Offset (Hz)                    	                1.0E9    G    This function enables or disables the marker at the active frequency.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    l    Specifies the active frequency.

Valid Values:

AUMG369XB_FREQUENCY_F0 - F0
AUMG369XB_FREQUENCY_F1 - F1 (Default)
AUMG369XB_FREQUENCY_F2 - F2
AUMG369XB_FREQUENCY_F3 - F3
AUMG369XB_FREQUENCY_F4 - F4
AUMG369XB_FREQUENCY_F5 - F5
AUMG369XB_FREQUENCY_F6 - F6 
AUMG369XB_FREQUENCY_F7 - F7
AUMG369XB_FREQUENCY_F8 - F8
AUMG369XB_FREQUENCY_F9 - F9
AUMG369XB_FREQUENCY_M0 - M0
AUMG369XB_FREQUENCY_M1 - M1
AUMG369XB_FREQUENCY_M2 - M2
AUMG369XB_FREQUENCY_M3 - M3
AUMG369XB_FREQUENCY_M4 - M4
AUMG369XB_FREQUENCY_M5 - M5
AUMG369XB_FREQUENCY_M6 - M6
AUMG369XB_FREQUENCY_M7 - M7
AUMG369XB_FREQUENCY_M8 - M8
AUMG369XB_FREQUENCY_M9 - M9     z    Enables or disables the marker at the active frequency.

Valid Values:

VI_FALSE - Disable (Default)
VI_TRUE  - Enabled    1����]  �    Status                           8R   o  �    Instrument Handle                8� -  u �    Active Frequency                 ;e - � i       Frequency Marker Enabled           	                         F0 AUMG369XB_FREQUENCY_F0 F1 AUMG369XB_FREQUENCY_F1 F2 AUMG369XB_FREQUENCY_F2 F3 AUMG369XB_FREQUENCY_F3 F4 AUMG369XB_FREQUENCY_F4 F5 AUMG369XB_FREQUENCY_F5 F6 AUMG369XB_FREQUENCY_F6 F7 AUMG369XB_FREQUENCY_F7 F8 AUMG369XB_FREQUENCY_F8 F9 AUMG369XB_FREQUENCY_F9 M0 AUMG369XB_FREQUENCY_M0 M1 AUMG369XB_FREQUENCY_M1 M2 AUMG369XB_FREQUENCY_M2 M3 AUMG369XB_FREQUENCY_M3 M4 AUMG369XB_FREQUENCY_M4 M5 AUMG369XB_FREQUENCY_M5 M6 AUMG369XB_FREQUENCY_M6 M7 AUMG369XB_FREQUENCY_M7 M8 AUMG369XB_FREQUENCY_M8 M9 AUMG369XB_FREQUENCY_M9     Enable VI_TRUE Disable VI_FALSE    4    This function configure the frequency marker mode.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    T    Configures the marker mode.

Valid Values:

AUMG369XB_FREQUENCY_MARKER_INTENSITY - Intensity
AUMG369XB_FREQUENCY_MARKER_VIDEO     - Video
AUMG369XB_FREQUENCY_MARKER_OFF       - Off (Default)

Notes:

When the video marker mode is turned on, the MG369XB generates a pulse output at each marker frequency that can be seen on a CRT display.
   ?A���]  �    Status                           E�   o  �    Instrument Handle                F� - � u �    Frequency Marker Mode              	                          yIntensity AUMG369XB_FREQUENCY_MARKER_INTENSITY Video AUMG369XB_FREQUENCY_MARKER_VIDEO Off AUMG369XB_FREQUENCY_MARKER_OFF    2    This function configures the power level unit.      �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Configures the power level unit. 

Valid Values:

AUMG369XB_UNIT_LOGARITHMIC_DBM (Default)    
AUMG369XB_UNIT_LOGARITHMIC_DBUV     
AUMG369XB_UNIT_LINEAR_MV            
   Ik���]  �    Status                           P)   o  �    Instrument Handle                P� - � u �    Power Level Unit                   	                           ddBm AUMG369XB_UNIT_LOGARITHMIC_DBM dBuV AUMG369XB_UNIT_LOGARITHMIC_DBUV mV AUMG369XB_UNIT_LINEAR_MV    0    This function configures the power level list.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Configures the L0 to L9 parameters. 

Valid Range: Power level range of the MG369XB model

Units: DM (Log), VT (Linear)

Notes:

(1) The array must contain 10 ViReal64 elements.   R����]  �    Status                           Y�   o  �    Instrument Handle                Z2 - � |  �    Power Level List                   	                    .    This function sets the selected power level.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    k    Selects the power level to be set.

Valid Values:

AUMG369XB_POWER_LEVEL_L0 - L0
AUMG369XB_POWER_LEVEL_L1 - L1 (Default)
AUMG369XB_POWER_LEVEL_L2 - L2
AUMG369XB_POWER_LEVEL_L3 - L3
AUMG369XB_POWER_LEVEL_L4 - L4
AUMG369XB_POWER_LEVEL_L5 - L5
AUMG369XB_POWER_LEVEL_L6 - L6
AUMG369XB_POWER_LEVEL_L7 - L7
AUMG369XB_POWER_LEVEL_L8 - L8
AUMG369XB_POWER_LEVEL_L9 - L9
     �    Specifies the power level value.

Valid Range: Power level range of the MG369XB model.

Default Value: 0.0

Units: DM (Log), VT (Linear)
   [����]  �    Status                           b�   o  �    Instrument Handle                c3 -  u �    Selected Power Level             d� - � {  �    Power Level                        	                       
  L0 AUMG369XB_POWER_LEVEL_L0 L1 AUMG369XB_POWER_LEVEL_L1 L2 AUMG369XB_POWER_LEVEL_L2 L3 AUMG369XB_POWER_LEVEL_L3 L4 AUMG369XB_POWER_LEVEL_L4 L5 AUMG369XB_POWER_LEVEL_L5 L6 AUMG369XB_POWER_LEVEL_L6 L7 AUMG369XB_POWER_LEVEL_L7 L8 AUMG369XB_POWER_LEVEL_L8 L9 AUMG369XB_POWER_LEVEL_L9    0.0    �    This function places the MG369XB in a fixed power level mode and   
configures the power level and the level offset.                        �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    j    Selects the power level to be set.

Valid Values:

AUMG369XB_POWER_LEVEL_L0 - L0
AUMG369XB_POWER_LEVEL_L1 - L1 (Default)
AUMG369XB_POWER_LEVEL_L2 - L2
AUMG369XB_POWER_LEVEL_L3 - L3
AUMG369XB_POWER_LEVEL_L4 - L4
AUMG369XB_POWER_LEVEL_L5 - L5
AUMG369XB_POWER_LEVEL_L6 - L6
AUMG369XB_POWER_LEVEL_L7 - L7
AUMG369XB_POWER_LEVEL_L8 - L8
AUMG369XB_POWER_LEVEL_L9 - L9        Turns on or turns off the level offset function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On 

Notes:

The Level Offset function lets you compensate for a device on the MG369XB's output that alters the RF output power level at the point of interest.     �    Sets the level offset constant to be applied to the level measured by the internal leveling loop.

Valid Range: -100 dB to 100 dB (Log), -xxx mV to xxx mV (Linear)

Default Value: 0.0

Units: DB (Log), VT (Linear)
   g����]  �    Status                           n�   o  �    Instrument Handle                o: -  u �    Selected Power Level             p� - � i       Level Offset Enabled             q� -� {  �    Level Offset                       	                       
  L0 AUMG369XB_POWER_LEVEL_L0 L1 AUMG369XB_POWER_LEVEL_L1 L2 AUMG369XB_POWER_LEVEL_L2 L3 AUMG369XB_POWER_LEVEL_L3 L4 AUMG369XB_POWER_LEVEL_L4 L5 AUMG369XB_POWER_LEVEL_L5 L6 AUMG369XB_POWER_LEVEL_L6 L7 AUMG369XB_POWER_LEVEL_L7 L8 AUMG369XB_POWER_LEVEL_L8 L9 AUMG369XB_POWER_LEVEL_L9     Enable VI_TRUE Disable VI_FALSE    0.0    �    This function sets the instrument power level to 1 dB below peak leveled power.                                                                    
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   u����]  �    Status                           |`   o  �    Instrument Handle                  	                �    This function places the MG369XB in a power level sweep mode and configures the dwellTime, the number of steps and the trigger source.                                                     
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     s    Sets the dwell-time-per-step of the power level sweep.

Valid Range: 0.001 to 99 

Default Value: 0.05

Unit: SEC     _    Sets the power sweep number of steps.

Valid Range: 1 to 10000

Default Value: 50

Unit: SPS
     �    Specifies the trigger source for the power level sweep.

Valid Values:

AUMG369XB_TRIGGER_SOURCE_AUTO     - Auto (Default)
AUMG369XB_TRIGGER_SOURCE_EXTERNAL - External
AUMG369XB_TRIGGER_SOURCE_SINGLE   - Single   ~?���]  �    Status                           ��   o  �    Instrument Handle                �� -  {  �    Dwell Time (Sec)                 � - � u �    Number of Steps                  �~ -� u �    Trigger Source                     	                0.05    50               uAuto AUMG369XB_TRIGGER_SOURCE_AUTO External AUMG369XB_TRIGGER_SOURCE_EXTERNAL Single AUMG369XB_TRIGGER_SOURCE_SINGLE   '    This function configures the leveling mode and the attenuation, enables or disables decoupling the step attenuator from the ALC. This function permits setting of the Reference Level DAC to control the ALC gain when in external power leveling mode.                                             
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    o    Sets the leveling mode of the output power.

Valid Values:

AUMG369XB_LEVELING_MODE_INTERNAL          - Internal (Default)
AUMG369XB_LEVELING_MODE_EXTERNAL_DETECTOR - External Dectector  
AUMG369XB_LEVELING_MODE_POWER_METER       - Power Meter  
AUMG369XB_LEVELING_MODE_FIXED_GAIN        - Fixed Gain  
AUMG369XB_LEVELING_MODE_AUTO_FIXED_GAIN   - Auto Fixed Gain  
    5    Selects or deselects ALC step attenuator decoupling.


Valid Values:

VI_FALSE - Deselect (Default)
VI_TRUE  - Select

Notes:

Selecting ALC step attenuator decoupling decouples the step attenuator (if equipped) from the ALC system. This permits separate setting the ALC level and step attenuator.

Deselecting the ALC step attenuator decoupling returns the MG369XB to its normal (coupled) leveling mode. In this mode, when the desired power level is set, the correct combination of ALC level and step attenuator setting is determined by the instrument firmware.
     �    Sets the step attenuator value to xx (times 10 dB) in the ALC steps attenuator decoupling mode.

Valid Range: 00 (0 dB attenuation) to 11 (110 dB attenuation)

Default Value: 00     �    Sets the Reference Level DAC to control the ALC gain when in external power leveling mode.

Valid Range: 0 to 255

Default Value: 0

Unit: SPS   �4���]  �    Status                           ��   o  �    Instrument Handle                �� -  u �    Leveling Mode                    � - � i       Decouple Enabled                 �E -� u  �    Attenuation                      �  �  u  �    Reference Level DAC                	                           �Internal AUMG369XB_LEVELING_MODE_INTERNAL External Dectector AUMG369XB_LEVELING_MODE_EXTERNAL_DETECTOR Power Meter AUMG369XB_LEVELING_MODE_POWER_METER Fixed Gain AUMG369XB_LEVELING_MODE_FIXED_GAIN Auto Fixed Gain AUMG369XB_LEVELING_MODE_AUTO_FIXED_GAIN     Enable VI_TRUE Disable VI_FALSE    0    0    �    This function turns on or turns off the ALC power slope function and configures the the pivot point frequency and the ALC power slope.   
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     g    Turns on or off the ALC power slope function.

Valid Values:

VI_FALSE - Off 
VI_TRUE  - On (Default)     ?    Sets the ALC power slope value.

Default Value: 0

Unit: SPS
    �    Sets the ALC power slope pivot point frequency parameter.

Valid Range: Frequency range of the MG369XB model.

Default Value: 0.0

Unit: HZ

Model Number     Configuration       Frequency Range
-------------------------------------------------------
  MG3691B      With Option 4 or 5     0.01 - 10.0 GHz
                  Standard            2.0 - 10.0 GHz
-------------------------------------------------------
  MG3692B      With Option 4 or 5     0.01 - 20.0 GHz
                  Standard            2.0 - 20.0 GHz
-------------------------------------------------------
  MG3693B      With Option 4 or 5     0.01 - 30.0 GHz
                  Standard            2.0 - 30.0 GHz
-------------------------------------------------------
  MG3694B      With Option 4 or 5     0.01 - 40.0 GHz
                  Standard            2.0 - 40.0 GHz
-------------------------------------------------------
  MG3695B      With Option 4 or 5     0.01 - 50.0 GHz
                  Standard            2.0 - 50.0 GHz
-------------------------------------------------------
  MG3696B      With Option 4 or 5     0.01 - 67.0 GHz
                  Standard            2.0 - 67.0 GHz   �����]  �    Status                           �q   o  �    Instrument Handle                � - � i       Slope Enabled                    � -� u  �    Slope                            �� -  {  �    Pivot (Hz)                         	                Enable VI_TRUE Disable VI_FALSE    0    0.0    �    This function enables or disables the power offset table and loads  a  power offset table.                                                 
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     e    Enables or disables the power offset table.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      v    Specifies the power offset list that be loaded into the table.

Valid Range: Power level range of the MG369XB model.     E    Specifies the size of the power offset list.

Valid Range: 1 - 1000   �A���]  �    Status                           ��   o  �    Instrument Handle                �� - F i       Power Offset Table Enabled       � -� |  �    Power Offset List                �� - � u  �    Number of Power Offset             	                Enable Enable Disable Disable            �    This function turns on or turns off the selected internal AM function and configures the depth, the rate and the waveform.                     
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     i    Turns on or turns off the internal AM function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Turns on the internal AM function in linear mode or log mode.

Valid Values:

AUMG369XB_AM_SCALING_LINEAR      - Linear (Default)
AUMG369XB_AM_SCALING_LOGARITHMIC - Log
     �    Sets the AM depth parameter.

Internal Linear AM
Valid Range: 0% to 100%
Default Value: 50%
Unit: PCT

Internal Log AM
Valid Range 0 to 25
Unit: DB     �    Sets the internal AM rate parameter.

Valid Range: 0.1 Hz to 1 MHz for sine wave; 0.1 Hz to 100 kHz for square, triangle, and ramp waveforms

Default Value: 10000.0

Unit:HZ
    �    Selects the internal AM waveform.

Valid Values:

AUMG369XB_WAVEFORM_SINE_WAVE             - Sine Wave (Default)
AUMG369XB_WAVEFORM_SQUARE_WAVE           - Square Wave
AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP  - Positive Ramp
AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP  - Negative Ramp
AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE - Guassian Noise
AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE  - Uniform Noise
AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE  - Triangle Wave
   �����]  �    Status                           �p   o  �    Instrument Handle                � - F i       Internal AM Enabled              �� - � u �    Scaling                          �3 -� {  �    Depth                            �� �  {  �    Rate (Hz)                        �� � � u �    Waveform                           	                 Enable VI_TRUE Disable VI_FALSE               HLinear AUMG369XB_AM_SCALING_LINEAR Log AUMG369XB_AM_SCALING_LOGARITHMIC    50.0    10000.0              bSine Wave AUMG369XB_WAVEFORM_SINE_WAVE Square Wave AUMG369XB_WAVEFORM_SQUARE_WAVE Positive Ramp AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP Negative Ramp AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP Guassian Noise AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE Uniform Noise AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE Triangle Wave AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE    �    This function turns on or turn off the selected external AM function and configures the sensitivity.                                          
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     i    Turns on or turns off the external AM function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Turns on the external AM function in linear mode or log mode.

Valid Values:

AUMG369XB_AM_SCALING_LINEAR      - Linear (Default)
AUMG369XB_AM_SCALING_LOGARITHMIC - Log
     �    Sets the eternal AM Sensitivity parameter

External Linear AM
Valid Range: 0 %/V to 100 %/V
Default Value: 100
Unit: PCV

External Log AM
Valid Range: 0 dB/V to 25 dB/V
Unit: DV
   �r���]  �    Status                           �0   o  �    Instrument Handle                �� - F i       External AM Enabled              �@ - � u �    Scaling                          �� -� {  �    Sensitivity                        	                 Enable VI_TRUE Disable VI_FALSE               HLinear AUMG369XB_AM_SCALING_LINEAR Log AUMG369XB_AM_SCALING_LOGARITHMIC    100.0    �    This function turns on or turns off the selected internal FM mode and configures the  deviation, the rate and the waveform.                 
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     i    Turns on or turns off the internal FM function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On         Turns on the internal FM function in selected mode.

Valid Values:

AUMG369XB_FM_MODE_LOCKED_LOW_NOISE  - Locked Low Noise
AUMG369XB_FM_MODE_LOCKED            - Locked (Default)
AUMG369XB_FM_MODE_UNLOCKED_NARROW   - Unlocked Narrow
AUMG369XB_FM_MODE_UNLOCKED_WIDE     - Unlocked Wide
     �    Sets the internal FM deviation parameter.

Valid Range: 

10 kHz to 20 MHz in Locked, Locked Low-Noise, and Unlocked Narrow FM; 100 kHz to 100 MHz in Unlocked Wide FM

Default Value: 1000000

Unit: HZ     �    Sets the internal FM rate parameter.

Valid Range: 
0.1 Hz to 1MHz for sine wave; 0.1 Hz to 100 kHz for square, triangle, and ramp waveforms.

Default Value: 100000

Unit: HZ
    �    Selects the internal FM waveform.

Valid Values: 

AUMG369XB_WAVEFORM_SINE_WAVE             - Sine Wave
AUMG369XB_WAVEFORM_SQUARE_WAVE           - Square Wave (Default)
AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP  - Square Positive Ramp
AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP  - Square Negative Ramp
AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE - Square Gaussian Noise
AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE  - Square Uniform Noise
AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE  - Square Triangle Wave
   �����]  �    Status                           Ԡ   o  �    Instrument Handle                �? - F i       Internal FM Enabled              հ - � u �    FM Mode                          �� -� {  �    Deviation (Hz)                   ר �  {  �    Rate (Hz)                        �a � � u �    Waveform                           	                 Enable VI_TRUE Disable VI_FALSE              �Locked Low Noise AUMG369XB_FM_MODE_LOCKED_LOW_NOISE Locked AUMG369XB_FM_MODE_LOCKED Unlocked Narrow AUMG369XB_FM_MODE_UNLOCKED_NARROW Unlocked Wide AUMG369XB_FM_MODE_UNLOCKED_WIDE    1.0E6    1.0E5              bSine Wave AUMG369XB_WAVEFORM_SINE_WAVE Square Wave AUMG369XB_WAVEFORM_SQUARE_WAVE Positive Ramp AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP Negative Ramp AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP Guassian Noise AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE Uniform Noise AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE Triangle Wave AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE    �    This function turns on or turns off the selected external FM mode and configures the sensitivity.                                       
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     i    Turns on or turns off the external FM function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On         Turns on the external FM function in selected mode.

Valid Values:

AUMG369XB_FM_MODE_LOCKED_LOW_NOISE  - Locked Low Noise
AUMG369XB_FM_MODE_LOCKED            - Locked (Default)
AUMG369XB_FM_MODE_UNLOCKED_NARROW   - Unlocked Narrow
AUMG369XB_FM_MODE_UNLOCKED_WIDE     - Unlocked Wide
     �    Sets the external FM sensitivity parameter.

Valid Range: 
+-10 kHz/V to +- 20 MHz/V in Locked, Locked Low-Noise, and Unlocked Narrow FM; +-100 kHz/V to +-100 MHz/V in Unlocked Wide FM.

Default Value: 1000.0 KV

Unit: KV   �����]  �    Status                           �   o  �    Instrument Handle                �1 - F i       External FM Enabled              � - � u �    FM Mode                          �� -� {  �    Sensitivity (KV)                   	                 Enable VI_TRUE Disable VI_FALSE              �Locked Low Noise AUMG369XB_FM_MODE_LOCKED_LOW_NOISE Locked AUMG369XB_FM_MODE_LOCKED Unlocked Narrow AUMG369XB_FM_MODE_UNLOCKED_NARROW Unlocked Wide AUMG369XB_FM_MODE_UNLOCKED_WIDE    1000.0    �    This function turns on or turns off the selected internal PM and configures the deviation, the rate and the waveform.            
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     i    Turns on or turns off the internal PM function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Turns on the internal PM function in narrow or wide mode.

Valid Values:

AUMG369XB_PM_MODE_NARROW - Narrow (Default)
AUMG369XB_PM_MODE_WIDE   - Wide
     �    Sets the internal PM deviation parameter.

Valid Range: 
0.0025 to 5 radians in Phase Modulation Narrow mode; 0.25 to 500 radians in Phase Modulation Wide mode.

Default Value: 1.0 

Unit: RD
     �    Sets the internal PM rate parameter.

Valid Range:
0.1 Hz to 1 MHz for sine wave; 0.1 Hz to 100 kHz for square, triangle, and ramp waveforms.

Default Value: 100000

Unit: HZ    �    Selects the internal PM waveform.

Valid Values: 

AUMG369XB_WAVEFORM_SINE_WAVE             - Sine Wave
AUMG369XB_WAVEFORM_SQUARE_WAVE           - Square Wave (Default)
AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP  - Square Positive Ramp
AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP  - Square Negative Ramp
AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE - Square Gaussian Noise
AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE  - Square Uniform Noise
AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE  - Square Triangle Wave
   �D���]  �    Status                           �   o  �    Instrument Handle                � - F i       Internal PM Enabled              � - � u �    PM Mode                          � -� {  �    Deviation (Radian)               �| �  {  �    Rate (Hz)                        �4 � � u �    Waveform                           	                 Enable VI_TRUE Disable VI_FALSE               <Narrow AUMG369XB_PM_MODE_NARROW Wide AUMG369XB_PM_MODE_WIDE    1.0    1.0E5              bSine Wave AUMG369XB_WAVEFORM_SINE_WAVE Square Wave AUMG369XB_WAVEFORM_SQUARE_WAVE Positive Ramp AUMG369XB_WAVEFORM_SQUARE_POSITIVE_RAMP Negative Ramp AUMG369XB_WAVEFORM_SQUARE_NEGATIVE_RAMP Guassian Noise AUMG369XB_WAVEFORM_SQUARE_GAUSSIAN_NOISE Uniform Noise AUMG369XB_WAVEFORM_SQUARE_UNIFORM_NOISE Triangle Wave AUMG369XB_WAVEFORM_SQUARE_TRIANGLE_WAVE    �    This function turns on or turns off the selected external PM and configures the sensitivity.                                      
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     i    Turns on or turns off the external PM function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Turns on the external PM function in narrow or wide mode.

Valid Values:

AUMG369XB_PM_MODE_NARROW - Narrow (Default)
AUMG369XB_PM_MODE_WIDE   - Wide
     �    Sets the external PM sensitivity parameter.

Valid Range:
+-0.0025 radians/V to +-5 radians/V in FM Narrow mode; +-0.25 radians/V to +-500 radians/V in FM Wide mode.

Default Value: 1.0

Unit: RV
   �'���]  �    Status                           �   o  �    Instrument Handle                � - F i       External PM Enabled              � - � u �    PM Mode                          � -� {  �    Sensitivity (Radians/V)            	                 Enable VI_TRUE Disable VI_FALSE               <Narrow AUMG369XB_PM_MODE_NARROW Wide AUMG369XB_PM_MODE_WIDE    1.0    �    This function turns on or turns off the selected internal pulse modulation and configures  the period, the trigger type, the trigger edge type, the polarity of the signal that turns the RF on and the clock rate.           
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     w    Turns on or turns off the internal pulse modulation function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On         Selects the internal pulse mode.

Valid Values:

AUMG369XB_INTERNAL_PULSE_MODE_SINGLE     - Single (Default)
AUMG369XB_INTERNAL_PULSE_MODE_DOUBLET    - Doublet 
AUMG369XB_INTERNAL_PULSE_MODE_TRIPLET    - Triplet
AUMG369XB_INTERNAL_PULSE_MODE_QUADRUPLET - Quadruplet
    ]    Selects the internal pulse trigger.

Valid Values:

AUMG369XB_TRIGGER_TYPE_FREE_RUN   - Free Run (Default)          
AUMG369XB_TRIGGER_TYPE_GATED      - Gated           
AUMG369XB_TRIGGER_TYPE_DELAYED    - Delayed          
AUMG369XB_TRIGGER_TYPE_TRIGGERED  - Triggered          
AUMG369XB_TRIGGER_TYPE_TRIGGERED_WITH_DELAY - Triggered with Delay
     �    Selects internal pulse triggering on the rising or falling edge of an external input.

Valid Values:

AUMG369XB_TRIGGER_EDGE_TYPE_RISING  - Trigger Rising (Default)
AUMG369XB_TRIGGER_EDGE_TYPE_FALLING - Trigger Falling
     �    Selects TTL-low or TTL-high to turn RF on during pulse modulation.

Valid Values:

AUMG369XB_RF_ON_POLARITY_LOW  - Low RF On  
AUMG369XB_RF_ON_POLARITY_HIGH - High RF On (Default)
     �    Selects the 10 MHz, the 100 MHz(Option 26) or the 40 MHz(Option 24) internal pulse clock rate.

Valid Values:

AUMG369XB_CLOCK_RATE_10MHZ        - Clock 10
AUMG369XB_CLOCK_RATE_40MHZ_100MHZ - Clock 100 (Default)
     �    Sets the internal pulse period parameter.

Valid Range:
40 ns to 167 ms at 100 MHz pulse clock rate; 250 ns to 419 ms at 40 MHz pulse clock rate; 600 ns to 1.6 s at 10 MHz pulse clock rate.

Default Value: 0.001 s

Unit: SEC
   ����]  �    Status                           �   o  �    Instrument Handle                7 - F i       Internal Pulse Mod Enabled       � -� u �    Mode                             � �  u �    Trigger Type                     / � � u �    Trigger Edge Type                 �� u �    Polarity                         � �  u �    Clock Rate                       � - � {  �    Period (Sec)                       	                 Enable VI_TRUE Disable VI_FALSE               �Single AUMG369XB_INTERNAL_PULSE_MODE_SINGLE Doublet AUMG369XB_INTERNAL_PULSE_MODE_DOUBLET Triplet AUMG369XB_INTERNAL_PULSE_MODE_TRIPLET Quadruplet AUMG369XB_INTERNAL_PULSE_MODE_QUADRUPLET               �Free Run AUMG369XB_TRIGGER_TYPE_FREE_RUN Gated AUMG369XB_TRIGGER_TYPE_GATED Delayed AUMG369XB_TRIGGER_TYPE_DELAYED Triggered AUMG369XB_TRIGGER_TYPE_TRIGGERED Triggered w/delay AUMG369XB_TRIGGER_TYPE_TRIGGERED_WITH_DELAY               fTrigger Rising AUMG369XB_TRIGGER_EDGE_TYPE_RISING Trigger Falling AUMG369XB_TRIGGER_EDGE_TYPE_FALLING              PLow RF On AUMG369XB_RF_ON_POLARITY_LOW High RF On AUMG369XB_RF_ON_POLARITY_HIGH              PClock 10 AUMG369XB_CLOCK_RATE_10MHZ Clock 100 AUMG369XB_CLOCK_RATE_40MHZ_100MHZ    0.001    :    This function configures the internal pulse frequency.      �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Sets the internal pulse frequency parameter.

Valid Range:
5.96 Hz to 25 MHz at 100 MHz pulse clock rate; 2.385 Hz to 4 MHz at 40 MHz pulse clock rate; 0.597 Hz to 1.66 MHz at 10 MHz pulse clock rate.

Default Value: 1000

Unit: HZ
   ����]  �    Status                            �   o  �    Instrument Handle                !< - � {  �    Frequency (Hz)                     	                1000.0    G    This function configures the selected internal pulse width parameter.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Specifies the internal pulse width to be set.

Valid Values:

AUMG369XB_INTERNAL_PULSE_WIDTH_1 - W1 (Default)
AUMG369XB_INTERNAL_PULSE_WIDTH_2 - W2
AUMG369XB_INTERNAL_PULSE_WIDTH_3 - W3
AUMG369XB_INTERNAL_PULSE_WIDTH_4 - W4
     �    Sets the selected internal pulse width parameter.

Valid Range:
30 ns to 167 ms with 100 MHz pulse clock rate; 25 ns to 419 ms at 40 MHz pulse clock rate; 100 ns to 1.6 s at 10 MHz pulse clock rate.

Default Value: 0.0001 s

Unit: SEC
   #6���]  �    Status                           )�   o  �    Instrument Handle                *� -  u �    Selected Pulse Width             +} - � {  �    Pulse Width (Sec)                  	                           �W1 AUMG369XB_INTERNAL_PULSE_WIDTH_1 W2 AUMG369XB_INTERNAL_PULSE_WIDTH_2 W3 AUMG369XB_INTERNAL_PULSE_WIDTH_3 W4 AUMG369XB_INTERNAL_PULSE_WIDTH_4    0.0001    G    This function configures the selected internal pulse delay parameter.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Specifies the internal pulse delay to be set.

Valid Values:

AUMG369XB_INTERNAL_PULSE_DELAY_1 - D1 (Default)
AUMG369XB_INTERNAL_PULSE_DELAY_2 - D2
AUMG369XB_INTERNAL_PULSE_DELAY_3 - D3
AUMG369XB_INTERNAL_PULSE_DELAY_4 - D4
     �    Sets the selected internal pulse delay parameter.

Valid Range:
30 ns to 167 ms at 100 MHz pulse clock rate; 100 ns to 419 ms at 40 MHz pulse clock rate; 300 ns to 1.6 s at 10 MHz clock rate.

Default Value: 0.0002 s

Unit: SEC
   .N���]  �    Status                           5   o  �    Instrument Handle                5� -  u �    Selected Pulse Delay             6� - � {  �    Pulse Delay (Sec)                  	                           �D1 AUMG369XB_INTERNAL_PULSE_DELAY_1 D2 AUMG369XB_INTERNAL_PULSE_DELAY_2 D3 AUMG369XB_INTERNAL_PULSE_DELAY_3 D4 AUMG369XB_INTERNAL_PULSE_DELAY_4    0.0002    �    This function enables or disables the step delay and configures the start delay, the stop delay, the step size and the step time.       
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     v    Turns on or turns off the internal pulse stepped delay mode.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Sets the internal pulse stepped delay mode delay 1 start parameter.

Valid Range:
0 ms to 167 ms at 100 MHz pulse clock rate; 0 ms to 419 ms at 40 MHz pulse clock rate; 0 s to 1.6 s at 10 MHz pulse clock rate.

Default Value: 0.0001 s

Unit: SEC
     �    Sets the internal pulse stepped delay mode delay 1 stop parameter.

Valid Range:
0 ms to 167 ms at 100 MHz pulse clock rate; 0 ms to 419 ms at 40 MHz pulse clock rate; 0 s to 1.6 s at 10 MHz pulse clock rate.

Default Value: 0.0005 s

Unit: SEC
     �    Sets the internal pulse stepped delay mode step size parameter.

Valid Range:
0 ms to 167 ms at 100 MHz pulse clock rate; 0 ms to 419 ms at 40 MHz pulse clock rate; 0 s to 1.6 s at 10 MHz pulse clock rate.

Default Value: 0.00001

Unit: SEC
     �    Sets the internal pulse stepped delay mode dwell-time-per-step.

Valid Range: 100 ms to 10 s

Default Value: 0.001 s

Unit: SEC
   9����]  �    Status                           @a   o  �    Instrument Handle                A  - F i       Enabled                          A~ - � {  �    Start Delay (Sec)                B~ -� {  �    Stop Delay (Sec)                 C} �  {  �    Step Size (Sec)                  Dx � � {  �    Step Time (Sec)                    	                 Enable VI_TRUE Disable VI_FALSE    0.0001    0.0005    0.00001    0.001    ?    This function turns on internal square wave pulse modulation.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    &    Turns on the selected internal square wave pulse modulation.

Valid Values:

AUMG369XB_SQUARE_WAVE_FREQUENCY_400_HZ   - 400 Hz (Default)
AUMG369XB_SQUARE_WAVE_FREQUENCY_1000_HZ  - 1 kHz
AUMG369XB_SQUARE_WAVE_FREQUENCY_7812_HZ  - 7.8125 kHz
AUMG369XB_SQUARE_WAVE_FREQUENCY_27800_HZ - 27.8 kHz
   G���]  �    Status                           M�   o  �    Instrument Handle                Nt - � u �    Square Wave Frequency              	                           �400 Hz AUMG369XB_SQUARE_WAVE_FREQUENCY_400_HZ 1 kHz AUMG369XB_SQUARE_WAVE_FREQUENCY_1000_HZ 7.8125 kHz AUMG369XB_SQUARE_WAVE_FREQUENCY_7812_HZ 27.8 kHz AUMG369XB_SQUARE_WAVE_FREQUENCY_27800_HZ    �    This function turns on or turns off the external pulse modulation and configures  the polarity of the signal (Low or High) that turns the RF on.
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     w    Turns on or turns off the external pulse modulation function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On      �    Selects TTL-low or TTL-high to turn RF on during pulse modulation.

Valid Values:

AUMG369XB_RF_ON_POLARITY_LOW  - Low RF On  
AUMG369XB_RF_ON_POLARITY_HIGH - High RF On (Default)   Q����]  �    Status                           Xz   o  �    Instrument Handle                Y - F i       External Pulse Mod Enabled       Y� - � u �    Polarity                           	                 Enable VI_TRUE Disable VI_FALSE              PLow RF On AUMG369XB_RF_ON_POLARITY_LOW High RF On AUMG369XB_RF_ON_POLARITY_HIGH    C    This function turns on or turns off the SCAN modulation function.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     m    Turns on or turns off the SCAN modulation function.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On    \���]  �    Status                           b�   o  �    Instrument Handle                cc - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    >    This function turns on or turns off the AM measurement mode.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None        Turns on or turns off the AM measurement mode.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On 

Notes:

In AM measurement mode, the signal generator measures the voltage of the external modulating signal and calculates the modulation depth of the RF output signal. 
   d����]  �    Status                           k�   o  �    Instrument Handle                lM - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    >    This function turns on or turns off the FM measurement mode.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None        Turns on or turns off the FM measurement mode.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On 

Notes:

In the FM measurement mode, the signal generator measures the voltage of the external modulating signal and calculates the peak frequency deviation of the RF output signal.   n����]  �    Status                           u?   o  �    Instrument Handle                u� - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    G    This function turns on or turns off the Power Meter measurement mode.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    j    Turns on or turns off the Power Meter measurement mode.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On 

Notes:

RF power can be measured at an external source with a 560-7, 5400-71, or 4600-71 series detector connected to the rear panel POWER METER connector. If the internal power meter option is not installed, this function produces a syntax error.   x%���]  �    Status                           ~�   o  �    Instrument Handle                � - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    �    This function configures the rear panel parameters, including the retrace and bandswitch blanking outputs polarity, the marker polarity, the internal penlift relay and the external reference loop bandwidth.        
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Selects +3.3 V or -3.3 V for retrace and bandswitch blanking outputs.

Valid Values:

AUMG369XB_BLANKING_POLARITY_POSITIVE - Positive (Default)
AUMG369XB_BLANKING_POLARITY_NEGATIVE - Negative 
     �    Sets the rear panel marker polarity positive or negative.

Valid Values:

AUMG369XB_MARKER_POLARITY_POSITIVE - Positive (Default)
AUMG369XB_MARKER_POLARITY_NEGATIVE - Negative
     �    Sets the penlift relay open or closed.

Valid Values:

AUMG369XB_PENLIFT_RELAY_NORMALLY_OPEN  - Normally Open (Default)
AUMG369XB_PENLIFT_RELAY_NORMALLY_CLOSED- Normally Closed
    %    Selects the external reference loop bandwidth.

Valid Values:

AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_10_HZ  - 10 Hz
AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_30_HZ  - 30 Hz
AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_100_HZ - 100 Hz (Default)
AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_300_HZ - 300 Hz
   �����]  �    Status                           �e   o  �    Instrument Handle                � -  u �    Blanking Polarity                �� - � u �    Marker Polarity                  �� -� u �    Penlift Relay                    �D �  u �    External Reference Bandwidth       	                           \Positive AUMG369XB_BLANKING_POLARITY_POSITIVE Negative AUMG369XB_BLANKING_POLARITY_NEGATIVE               XPositive AUMG369XB_MARKER_POLARITY_POSITIVE Negative AUMG369XB_MARKER_POLARITY_NEGATIVE               lNormally Open AUMG369XB_PENLIFT_RELAY_NORMALLY_OPEN Normally Closed AUMG369XB_PENLIFT_RELAY_NORMALLY_CLOSED              �10 Hz AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_10_HZ 30 Hz AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_30_HZ 100 Hz AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_100_HZ 300 Hz AUMG369XB_EXTERNAL_REFERENCE_BANDWIDTH_300_HZ    6    This function turns on or turns off the secure mode.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     `    Turns on or turns off the secure mode.

Valid Values:

VI_FALSE - Off (Default)
VI_TRUE  - On    �)���]  �    Status                           ��   o  �    Instrument Handle                �� - � i       Enabled                            	                 Enable VI_TRUE Disable VI_FALSE    �    This function configures MG369XB to execute selected command each time a GET message is received. You can call the aumg369xb_sendGETBusMessage function to send a GET message.                                             
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    /    Configures the MG369XB to execute a selected command each time a GET message is received.

Valid Values:

AUMG369XB_EXECUTE_COMMAND_OFF  - Disable GET Function (Default)
AUMG369XB_EXECUTE_COMMAND_SQF  - Execute SQF Command
AUMG369XB_EXECUTE_COMMAND_DN   - Execute DN Command
AUMG369XB_EXECUTE_COMMAND_FFSS - Execute Fast-Frequency-Switching Step
AUMG369XB_EXECUTE_COMMAND_TSS  - Execute TSS Command
AUMG369XB_EXECUTE_COMMAND_TRS  - Execute TRS Command
AUMG369XB_EXECUTE_COMMAND_TST  - Execute TST Command
AUMG369XB_EXECUTE_COMMAND_UP   - Execute UP Command
   �����]  �    Status                           �d   o  �    Instrument Handle                � - � u �    Execute Command                    	                          �Disable GET Function AUMG369XB_EXECUTE_COMMAND_OFF Execute SQF Command AUMG369XB_EXECUTE_COMMAND_SQF Execute DN Command AUMG369XB_EXECUTE_COMMAND_DN Execute Fast-Frequency-Switching Step AUMG369XB_EXECUTE_COMMAND_FFSS Execute TSS Command AUMG369XB_EXECUTE_COMMAND_TSS Execute TRS Command AUMG369XB_EXECUTE_COMMAND_TRS Execute TST Command AUMG369XB_EXECUTE_COMMAND_TST Execute UP Command AUMG369XB_EXECUTE_COMMAND_UP    6    This function scans to the next preset CW frequency.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Specifies the type of the preset CW frequency scan.

Valid Values:

AUMG369XB_FREQUENCY_SCAN_DOWN_LOWER  - Scan Down (Default)
AUMG369XB_FREQUENCY_SCAN_UP_HIGHER   - Scan Up
AUMG369XB_FREQUENCY_SCAN_NEXT_HIGHER - Scan Next   �����]  �    Status                           ��   o  �    Instrument Handle                �3 - � u �    Scan Frequency Type                	                           �Scan Down AUMG369XB_FREQUENCY_SCAN_DOWN_LOWER Scan Up AUMG369XB_FREQUENCY_SCAN_UP_HIGHER Scan Next AUMG369XB_FREQUENCY_SCAN_NEXT_HIGHER    /    This function zeros the phase offset display.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �����]  �    Status                           �V   o  �    Instrument Handle                  	                �    This function configures the MG369XB to perform calculations to set the frequency and power levels during the initial list sweep.               
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �
���]  �    Status                           ��   o  �    Instrument Handle                  	                   This function sets the internal pulse mode to single, the internal pulse trigger to triggered with delay, turns on the internal pulse modulation function, and disables the external pulse modulation function, if previously programmed.                                                  
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   ����]  �    Status                           ��   o  �    Instrument Handle                  	                �    This function sets the internal pulse mode to single, the internal pulse trigger to gated, turns on the internal pulse modulation function, and disables the external pulse modulation function, if previously programmed.
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �����]  �    Status                           с   o  �    Instrument Handle                  	               '    This function sets the internal pulse width to 500ms, PRF to 1 kHz, pulse mode to single, pulse trigger to free run, turns on the internal pulse modulation function, and disables the external pulse modulation function, if previously programmed.                                                
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �����]  �    Status                           ڇ   o  �    Instrument Handle                  	                �    This function performs a master reset of the instrument by setting all of the instrument parameters to factory default.                             
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �?���]  �    Status                           ��   o  �    Instrument Handle                  	                �    This function triggers a single sweep. When single sweep triggering is selected for frequency, power level, or list sweeps, a single sweep starts when the TRG command is received.                               
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �����]  �    Status                           �   o  �    Instrument Handle                  	                �    This function resets the sweep to its start frequency (or power level), whenever the command is received while a single sweep is in progress.  
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �d���]  �    Status                           �"   o  �    Instrument Handle                  	               $    This function advances sweep to the next step in dual step sweep mode. The dual step sweep mode provides for generating synchronized, step sweep outputs for two MG369XBx at a frequency offset. When operating in the dual step sweep mode, the command, TSS, steps the sweep to the next point.
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �g���]  �    Status                           �%   o  �    Instrument Handle                  	               /    This function sends a Y to the MG369XB. Sending a Y is equivalent to sending a GET (Group Execute Trigger) message.
In the default state, the MG369XB responds to a GET message by triggering a single sweep. You can configure the response by calling the aumg369xb_configureGroupExecuteTrigger function.
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �u���]  �    Status                           3   o  �    Instrument Handle                  	                �    This function resets the MG369XB to its default settings. Sending this command clears the current instrument setup.                           
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   ����]  �    Status                           �   o  �    Instrument Handle                  	                I    This function executes the instrument self-test and returns the result.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     K    Returns the result of the self test.

Returned Values:
0 - Pass
-1- Fail
     ^    Returns six self test results bytes. 

Notes:

(1) The array must contain 6 ViByte elements.   ���]  �    Status                           �   o  �    Instrument Handle                j -  u  �    Test Result                      � - � �  �    Test Result Bytes                  	                	            	            >    This function queries the instrument last GPIB syntax error.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     g    Returns the last GPIB syntax error.

Notes:

(1) The array must contain at least 256 ViChar elements.   e���]  �    Status                            #   o  �    Instrument Handle                 � - � �  �    Error Message                      	                	            �    This function translates the error return value from the instrument driver into a user-readable string.                                  
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     t    This control accepts the Status Code returned from the instrument driver functions.

Default Value: 0 - VI_SUCCESS     �    This control returns the interpreted Status Code as a user readable message string.

Notes:

(1) The array must contain at least 256 ViChar elements.   "����]  �    Status                           )>   o  �    Instrument Handle                )� -  ]  �    Status Code                      *Y - � �  �    Message                            	                0    	            <    This function returns the driver and instrument revisions.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    This control returns the Instrument Driver Software Revision.

Notes:

(1) The array must contain at least 256 ViChar elements.     z    This control returns the Instrument Firmware Revision.

Notes:

(1) The array must contain at least 256 ViChar elements.     z    This control returns the Instrument Firmware Revision.

Notes:

(1) The array must contain at least 256 ViChar elements.   ,1���]  �    Status                           2�   o  �    Instrument Handle                3� -  �  �    Driver Revision                  4 - � �  �    Build Revision                   4� -� �  �    Instrument Revision                	                	            	            	            ;    This function returns the model number of the instrument.     �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    Returns the model number of current instrument.

Return Values:

AUMG369XB_MODEL_MG3691B 
AUMG369XB_MODEL_MG3692B 
AUMG369XB_MODEL_MG3693B 
AUMG369XB_MODEL_MG3694B 
AUMG369XB_MODEL_MG3695B 
AUMG369XB_MODEL_MG3696B 
   6�   o  �    Instrument Handle                7:���]  �    Status                           =� - � u  �    Model Number                           	            	            8    This function configures the instrument serial number.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     f    Sets the instrument serial number.

Notes:

(1) The serial number must be six characters in length.
   ?����]  �    Status                           F�   o  �    Instrument Handle                G1 - � �  �    Serial Number                      	                    F    This function returns the instrument to local (front panel) control.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   H����]  �    Status                           O^   o  �    Instrument Handle                  	                L    This function saves the current instrument setup to internal setup memory.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None         Saves the current instrument setup to internal setup memory location.

Valid Range: memory location 1 to 9

Default Value: 1
   P����]  �    Status                           W�   o  �    Instrument Handle                X( - � u  �    Setup Number                       	                1    �    This function recalls the instrument setup stored in internal setup memory.                                                            
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     p    Recalls the instrument setup stored in internal setup memory location.

Valid Range: 1 to 9

Default Value: 1
   Y����]  �    Status                           `�   o  �    Instrument Handle                aR - � u  �    Setup Number                       	                1    E    This function recalls the next stored instrument setup in sequence.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   b����]  �    Status                           i�   o  �    Instrument Handle                  	                B    This function prepares the MG369XB to receive instrument setups.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Selects the instrument setup type.

Valid Values:

AUMG369XB_INSTRUMENT_SETUP_CURRENT - Current Instrument Setup
AUMG369XB_INSTRUMENT_SETUP_ALL     - All Instrument Setup 

Default Value: Current Instrument Setup     &    Specifies the instrument setup data.     4    Specifies the length of the instrument setup data.   j����]  �    Status                           q�   o  �    Instrument Handle                rI -  u �    Instrument Setup Type            s' -� �  �    Instrument Setup                 sU - � u  �    Array Length                       	                           pCurrent Instrument Setup AUMG369XB_INSTRUMENT_SETUP_CURRENT All Instrument Setup AUMG369XB_INSTRUMENT_SETUP_ALL            �    This function outputs the instrument setups to the computer in a 4100-byte (approximately) binary data string.                                       
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     �    Selects the instrument setup type.

Valid Values:

AUMG369XB_INSTRUMENT_SETUP_CURRENT - Current Instrument Setup
AUMG369XB_INSTRUMENT_SETUP_ALL     - All Instrument Setup 

Default Value: Current Instrument Setup     $    Returns the instrument setup data.     <    Specifies the capacity of the instrument setup data array.     ;    Returns the length of the received instrument setup data.   u����]  �    Status                           |�   o  �    Instrument Handle                }. -  u �    Instrument Setup Type            ~ -� �  �    Instrument Setup                 ~8 - � u  �    Array Length                     ~| �  u  �    Return Length                      	                           pCurrent Instrument Setup AUMG369XB_INSTRUMENT_SETUP_CURRENT All Instrument Setup AUMG369XB_INSTRUMENT_SETUP_ALL    	                	            D    This function activates the selected user level calibration table.    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None    +    Activates the selected user level calibration table.

Valid Values:

AUMG369XB_CALIBRATION_TABLE_OFF - Off (Default)
AUMG369XB_CALIBRATION_TABLE_1   - 1
AUMG369XB_CALIBRATION_TABLE_2   - 2
AUMG369XB_CALIBRATION_TABLE_3   - 3
AUMG369XB_CALIBRATION_TABLE_4   - 4
AUMG369XB_CALIBRATION_TABLE_5   - 5
   �����]  �    Status                           ��   o  �    Instrument Handle                �Q - � u �    Calibration Table Number           	                           �Off AUMG369XB_CALIBRATION_TABLE_OFF 1 AUMG369XB_CALIBRATION_TABLE_1 2 AUMG369XB_CALIBRATION_TABLE_2 3 AUMG369XB_CALIBRATION_TABLE_3 4 AUMG369XB_CALIBRATION_TABLE_4 5 AUMG369XB_CALIBRATION_TABLE_5    �    This function readies the MG369XB to receive the five tables of user level calibration data that were stored in the controller by the LUS command.                                                             
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     ?    Specifies all five tables of the user level calibration data.     K    Specifies the length of the calibration data array.

Default Value: 10000   �����]  �    Status                           ��   o  �    Instrument Handle                �= - � �  �    Calibration Data                 �� -  u  �    Array Length                       	                    10000    �    This function sends all five tables of of user level calibration data to the controller where they are stored in a binary data file.       
    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None     =    Returns all five tables of the user level calibration data.     M    Specifies the capacity of the calibration data array.

Default Value: 10000     6    Returns the length of the received calibration data.   �^���]  �    Status                           �   o  �    Instrument Handle                �� - � �  �    Calibration Data                 �  -  u  �    Array Length                     �U -� u  �    Return Length                      	                	            10000    	            b    This function performs the following operations:
viClose (instrSession) and viClose (rmSession).    �    This control contains the status code returned by the function call.

Status Codes:

Status    Description
-------------------------------------------------
       0  No error (the call was successful).

3FFC0101  ID Query not supported - VI_WARN_NSUP_ID_QUERY   
3FFC0102  Reset not supported - VI_WARN_NSUP_RESET      
3FFC0103  Self Test not supported - VI_WARN_NSUP_SELF_TEST  
3FFC0104  Error Query not supported - VI_WARN_NSUP_ERROR_QUERY
3FFC0105  Revision Query not supported - VI_WARN_NSUP_REV_QUERY  
3FFF0005  The specified termination character was read.
3FFF0006  The specified number of bytes was read.

BFFC0001  Parameter 1 out of range. 
BFFC0002  Parameter 2 out of range.
BFFC0003  Parameter 3 out of range.
BFFC0004  Parameter 4 out of range.
BFFC0005  Parameter 5 out of range.
BFFC0006  Parameter 6 out of range.
BFFC0007  Parameter 7 out of range.
BFFC0008  Parameter 8 out of range.
BFFC0011  Instrument returned invalid response to ID Query

BFFF0000  Miscellaneous or system error occurred.
BFFF000E  Invalid session handle.
BFFF0015  Timeout occurred before operation could complete.
BFFF0034  Violation of raw write protocol occurred.
BFFF0035  Violation of raw read protocol occurred.
BFFF0036  Device reported an output protocol error.
BFFF0037  Device reported an input protocol error.
BFFF0038  Bus error occurred during transfer.
BFFF003A  Invalid setup (attributes are not consistent).
BFFF005F  No listeners condition was detected.
BFFF0060  This interface is not the controller in charge.
BFFF0067  Operation is not supported on this session.

Notes:

(1) Parameter Error Codes for parameters 1 through 8 are defined in the vpptype.h header file the range is BFFC0001 - BFFC0008;      �    This control accepts the Instrument Handle returned by the Initialize function to select the desired instrument driver session.

Default Value:  None   �7���]  �    Status                           ��   o  �    Instrument Handle                  	             ����         _  
             K.        init                                                                            _VI_FUNC                                                ����          v  ({             K.        configureOutputEnabled                                                          _VI_FUNC                                                ����         )M  5|             K.        configureRF                                                                     _VI_FUNC                                                ����         7�  D�             K.        configureFrequencyList                                                          _VI_FUNC                                                ����         EQ  S�             K.        configureFrequencyEntry                                                         _VI_FUNC                                                ����         V�  c             K.        configureDeltaFrequency                                                         _VI_FUNC                                                ����         c�  p1             K.        configureCWFrequency                                                            _VI_FUNC                                                ����         s�  �m             K.        configureFastFrequencySwitching                                                 _VI_FUNC                                                ����         ��  �f             K.        configureAnalogSweepFrequency                                                   _VI_FUNC                                                ����         �9  �             K.        configureStepSweepFrequency                                                     _VI_FUNC                                                ����         ��  ��             K.        configureStepSweepSweepTime                                                     _VI_FUNC                                                ����         �Q  �C             K.        configureAlternateSweepFrequency                                                _VI_FUNC                                                ����         �m  �i             K.        configureDualStepSweepEnabled                                                   _VI_FUNC                                                ����         �;  �c             K.        configureNonEquallySpacedStepSweepEnabled                                       _VI_FUNC                                                ����         �5  ��             K.        configureManualSweepFrequency                                                   _VI_FUNC                                                ����         ױ  �             K.        configureListSweepFrequency                                                     _VI_FUNC                                                ����         ��  ��             K.        configureSweepList                                                              _VI_FUNC                                                ����         �� �             K.        configureMasterSlaveOperation                                                   _VI_FUNC                                                ����        	L �             K.        configureSlaveFrequencyList                                                     _VI_FUNC                                                ����        q %             K.        configureSlaveFrequencyEntry                                                    _VI_FUNC                                                ����        ( 0�             K.        configureVNAMode                                                                _VI_FUNC                                                ����        1E ;�             K.        configureFrequencyMarkerEnabled                                                 _VI_FUNC                                                ����        ? G�             K.        configureFrequencyMarkerMode                                                    _VI_FUNC                                                ����        I1 Q{             K.        configurePowerLevelUnit                                                         _VI_FUNC                                                ����        R� Z�             K.        configurePowerLevelList                                                         _VI_FUNC                                                ����        [� e9             K.        configurePowerLevelEntry                                                        _VI_FUNC                                                ����        gK r�             K.        configureFixedPowerLevel                                                        _VI_FUNC                                                ����        u |�             K.        configure1dBBelowPeakLeveledPower                                               _VI_FUNC                                                ����        }y �Z             K.        configurePowerLevelSweep                                                        _VI_FUNC                                                ����        � ��             K.        configureALCMode                                                                _VI_FUNC                                                ����        � �c             K.        configureALCLoop                                                                _VI_FUNC                                                ����        �� ��             K.        configurePowerOffsetTable                                                       _VI_FUNC                                                ����        � �X             K.        configureInternalAM                                                             _VI_FUNC                                                ����        �� ˯             K.        configureExternalAM                                                             _VI_FUNC                                                ����        �K �U             K.        configureInternalFM                                                             _VI_FUNC                                                ����        �A �             K.        configureExternalFM                                                             _VI_FUNC                                                ����        � �(             K.        configureInternalPM                                                             _VI_FUNC                                                ����        �� c             K.        configureExternalPM                                                             _VI_FUNC                                                ����        � �         	    K.        configureInternalPulseModulation                                                _VI_FUNC                                                ����        � ".             K.        configureInternalPulseModulationFrequency                                       _VI_FUNC                                                ����        "� ,r             K.        configureInternalPulseWidth                                                     _VI_FUNC                                                ����        -� 7�             K.        configureInternalPulseDelay                                                     _VI_FUNC                                                ����        9 E             K.        configureInternalPulseStepDelay                                                 _VI_FUNC                                                ����        F� O�             K.        configureInternalSquareWavePulseModulation                                      _VI_FUNC                                                ����        Q! ZU             K.        configureExternalPulseModulation                                                _VI_FUNC                                                ����        [� c�             K.        configureSCANModulationEnabled                                                  _VI_FUNC                                                ����        d� mi             K.        configureAMMeasurementEnabled                                                   _VI_FUNC                                                ����        n; w             K.        configureFMMeasurementEnabled                                                   _VI_FUNC                                                ����        w� ��             K.        configurePowerMeterMeasurementEnabled                                           _VI_FUNC                                                ����        �� �q             K.        configureRearPanel                                                              _VI_FUNC                                                ����        �� ��             K.        configureSecureModeEnabled                                                      _VI_FUNC                                                ����        �� �:             K.        configureGroupExecuteTrigger                                                    _VI_FUNC                                                ����        �� �             K.        scanPresetCWFrequency                                                           _VI_FUNC                                                ����        �a ��             K.        zeroPhaseOffsetDisplay                                                          _VI_FUNC                                                ����        �o �g             K.        calculateSweepList                                                              _VI_FUNC                                                ����        �� �d             K.        setInternalTriggeredPulseWithDelay                                              _VI_FUNC                                                ����        �� �              K.        setInternalGatedPulse                                                           _VI_FUNC                                                ����        Қ �&             K.        setInternal1kHzSquareWavePulse                                                  _VI_FUNC                                                ����        ۠ �             K.        performMasterReset                                                              _VI_FUNC                                                ����        � �P             K.        triggerSingleSweep                                                              _VI_FUNC                                                ����        �� ��             K.        resetSweep                                                                      _VI_FUNC                                                ����        �; ��             K.        stepToNextSweep                                                                 _VI_FUNC                                                ����        �> �             K.        sendGETBusMessage                                                               _VI_FUNC                                                ����        L B             K.        reset                                                                           _VI_FUNC                                                ����        � #             K.        selfTest                                                                        _VI_FUNC                                                ����         !1             K.        errorQuery                                                                      _VI_FUNC                                                ����        !� *�             K.        errorMessage                                                                    _VI_FUNC                                                ����        +� 5             K.        revisionQuery                                                                   _VI_FUNC                                                ����        6X >�             K.        queryModelNumber                                                                _VI_FUNC                                                ����        ?� G�             K.        configureSerialNumber                                                           _VI_FUNC                                                ����        HR O�             K.        returnLocalControl                                                              _VI_FUNC                                                ����        Pw X�             K.        saveInstrumentSetup                                                             _VI_FUNC                                                ����        Yc a�             K.        recallInstrumentSetup                                                           _VI_FUNC                                                ����        b~ j(             K.        recallNextInstrumentSetup                                                       _VI_FUNC                                                ����        j� s�             K.        writeInstrumentSetup                                                            _VI_FUNC                                                ����        u1 ~�             K.        readInstrumentSetup                                                             _VI_FUNC                                                ����        �� ��             K.        configureUserLevelCalibration                                                   _VI_FUNC                                                ����        � ��             K.        writeUserLevelCalibrationTable                                                  _VI_FUNC                                                ����        �� ��             K.        readUserLevelCalibrationTable                                                   _VI_FUNC                                                ����        �� ��             K.        close                                                                           _VI_FUNC                                                      �                                                                                     �Initialize                                                                          Configuration Functions                                                             �RF                                                                                   �Configure Output Enabled                                                             �Configure RF                                                                        .Frequency                                                                            �Configure Frequency List                                                             �Configure Frequency Entry                                                            �Configure Delta Frequency                                                            �Configure CW Frequency                                                               �Configure Fast Frequency Switching                                                   �Configure Analog Sweep Frequency                                                     �Configure Step Sweep Frequency                                                       �Configure Step Sweep Sweep Time                                                      �Configure Alternate Sweep Frequency                                                  �Configure Dual Step Sweep Enabled                                                    �Configure Non Equally Spaced Step Sweep Enabled                                      �Configure Manual Sweep Frequency                                                     �Configure List Sweep Frequency                                                       �Configure Sweep List                                                                 �Configure Master Slave Operation                                                     �Configure Slave Frequency List                                                       �Configure Slave Frequency Entry                                                      �Configure VNA Mode                                                                   �Configure Frequency Marker Enabled                                                   �Configure Frequency Marker Mode                                                     �Level                                                                                �Configure Power Level Unit                                                           �Configure Power Level List                                                           �Configure Power Level Entry                                                          �Configure Fixed Power Level                                                          �Configure 1 dB Below Peak Leveled Power                                              �Configure Power Level Sweep                                                          �Configure ALC Mode                                                                   �Configure ALC Loop                                                                   �Configure Power Offset Table                                                        �Modulation                                                                           �Configure Internal AM                                                                �Configure External AM                                                                �Configure Internal FM                                                                �Configure External FM                                                                �Configure Internal PM                                                                �Configure External PM                                                                �Configure Internal Pulse Modulation                                                  �Configure Internal Pulse Modulation Frequency                                        �Configure Internal Pulse Width                                                       �Configure Internal Pulse Delay                                                       �Configure Internal Pulse Step Delay                                                  �Configure Internal Square Wave Pulse Modulation                                      �Configure External Pulse Modulation                                                  �Configure SCAN Modulation Enabled                                                   DMeasurement                                                                          �Configure AM Measurement Enabled                                                     �Configure FM Measurement Enabled                                                     �Configure Power Meter Measurement Enabled                                           �System                                                                               �Configure Rear Panel                                                                 �Configure Secure Mode Enabled                                                        �Configure Group Execute Trigger                                                     �Action/Status Functions                                                              �Scan Preset CW Frequency                                                             �Zero Phase Offset Display                                                            �Calculate Sweep List                                                                 �Set Internal Triggered Pulse With Delay                                              �Set Internal Gated Pulse                                                             �Set Internal 1 kHz Square Wave Pulse                                                 �Perform Master Reset                                                                 �Trigger Single Sweep                                                                 �Reset Sweep                                                                          �Step To Next Sweep                                                                   �Send GET Bus Message                                                                	HUtility Functions                                                                    �Reset                                                                                �Self Test                                                                            �Error Query                                                                          �Error Message                                                                        �Revision Query                                                                       �Query Model Number                                                                   �Configure Serial Number                                                              �Return Local Control                                                                
�Instrument Setup                                                                     �Save Instrument Setup                                                                �Recall Instrument Setup                                                              �Recall Next Instrument Setup                                                         �Write Instrument Setup                                                               �Read Instrument Setup                                                               User level Calibration                                                               �Configure User Level Calibration                                                     �Write User Level Calibration Table                                                   �Read User Level Calibration Table                                                    �Close                                                                           