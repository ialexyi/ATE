ModularZT.dll - ActiveX COM Object for programmers

This DLL file can be used by programmers of Visual Basic,VC++,Delphi,C#
LabView or any other program that recognize ActiveX COM Object DLL file.

ModularZT.dll file should be located on system folder eg: "c:\windows\system32"
the file should be registered using regsvr32 windows program.

The DLL file include the following Functions:

1.a int Connect(Optional *string SN)  :

SN parameter is needed in case of using more than 1 ZT-156.
SN is the Serial Number of the ZT-156 and can be ignored if using only one box.

1.b int ConnectByAddress(Optional short Address)  :

Address parameter is needed if more than 1 Switch Box are connected to PC.
and you want to connect to device by Address instead of SN.
This is an alternative to option 1.a (connect by SN).

Address parameter is the Address of the Modular ZT.
The address can be any integer number between 1 to 255 and can be changed by software.


2. void Disconnect()

Recommended to Disconnect the device while end of program




3. int Read_ModelName(string ModelName) As Integer
The Model Name returned in ModelName parameter.

4. int Read_SN(string SN ) 
The Serial Number returned in SN parameter.



   
5. int Set_Address(Address As Integer)   
   
   set the address of the unit. the address can be any number between 1 to 255.
   return positive number if success.
   
6. int Get_Address() 

   return the device address.
   if fail - return 0.
   
7. Int Get_Available_SN_List(string SN_List)

string SN_List is returned with all avaliable Switch Boxes connected to USB.
the function return the Number of ZT-156 units.

8. Int Get_Available_Address_List(string Add_List)

string Add_List is returned with all avaliable Switch Boxes connected to USB.
the function return the Number of ZT-156 units.   
   

9. GetUSBConnectionStatus - return the USB Connection Status.

12. int Send_SCPI(String SndSTR , String RetSTR)

this function recieve a SndSTR as parameter and return an "Answer" to
RetSTR. thid function return 1 for success and 0 for fail.



for example: 
MyZT is object of ModularZT.USB_Control
status = ModularZT.Send_SCPI("RUDAT:1A:ATT:75.75",RetSTR)
this example Set attenuation of SLOT1-A RUDAT  at 75.75 dB
it return  ResSTR = "1 - Success" in case of success.

 


13. int GetEthernet_CurrentConfig(int ip1,int ip2,int ip3,int ip4,int mask1,int mask2,int mask3,int mask4,int Gateway1,int Gateway2,int Gateway3,int Gateway4)

      return 1 for success 
             0 for fail
      this function return to the parameters (ip1 - ip4, mask1 - mask4,Gateway1 - Gateway4)
      the Ethernet configuration which stored.

14.int GetEthernet_IPAddress(int b1,int b2,int b3,int b4)
	
      return 1 for success 
             0 for fail
      this function return to b1 - b4 the static IP Address which stored.

15. int GetEthernet_MACAddress(int mac1,int mac2,int mac3,int mac4,int mac5,int mac6)		
	
	return 1 for success 
             0 for fail
	this function return to mac1 - mac6 the Mac Address which stored.
  
16. int GetEthernet_NetworkGateway(int b1,int b2,int b3,int b4)

	return 1 for success 
             0 for fail
	this function return to b1 - b4 the Network Gateway which stored.

17. int GetEthernet_PWD(String PWD)
	
	return 1 for success 
             0 for fail
	this function return the Ethernet Password which stored (if there is no password PWD = "")


18 int GetEthernet_SubNetMask(int b1,int b2,int b3,int b4)

	return 1 for success 
             0 for fail
	this function return to b1 - b4 the Ethernet SubnetMask which stored.

19. int GetEthernet_TCPIPPort(int Port)
	
	
	return 1 for success 
               0 for fail
	this function return to Port the TCP/IP Port which stored.

20. int GetEthernet_UseDHCP()

	return 1 for using DHCP (Dynamic IP)
        return 0 for not using DHCP (Static IP)

21  int GetEthernet_UsePWD()

	return 1 for using  Ethernet Password.
	return 0 for not using  Ethernet Password.

22. int SaveEthernet_IPAddress(int b1,int b2,int b3,int b4)

	return 1 for success 
               0 for fail
	store the IPAddress (b1.b2.b3.b4)

23. int SaveEthernet_NetworkGateway(int b1,int b2,int b3,int b4)

	return 1 for success 
               0 for fail
	store the Gateway (b1.b2.b3.b4)

24. int SaveEthernet_SubnetMask(int b1,int b2,int b3,int b4)

	return 1 for success 
               0 for fail
	store the SubnetMask (b1.b2.b3.b4)

25. int SaveEthernet_PWD(String PWD)
	
	return 1 for success 
               0 for fail
	store the Password (PWD)

26. int SaveEthernet_TCPIPPort(int Port)

	return 1 for success 
               0 for fail
	store the TCP/IP Port (Port)

27.  int SaveEthernet_UseDHCP(int UseDHCP)

	return 1 for success 
               0 for fail
	store UseDHCP=1 for using DHCP (Dynamic IP)
              UseDHCP=0 for not using DHCP (Static IP)

28. int SaveEthernet_UsePWD(int UsePWD)

	return 1 for success 
               0 for fail
	store UsePWD=1 for using Password
              UsePWD=0 for not using Password

