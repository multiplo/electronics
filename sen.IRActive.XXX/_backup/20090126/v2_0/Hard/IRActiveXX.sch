EESchema Schematic File Version 2
LIBS:power,device,transistors,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,opto,contrib,valves,.\IRActiveXX.cache
EELAYER 24  0
EELAYER END
$Descr A4 11700 8267
Sheet 1 1
Title ""
Date "21 jan 2009"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	4450 2000 3800 2000
Wire Wire Line
	4950 2400 4950 2000
Wire Wire Line
	4950 2000 5400 2000
Wire Wire Line
	3800 2400 4450 2400
$Comp
L OPTO_NPN Q1
U 1 1 49776C5E
P 3700 2200
F 0 "Q1" H 3850 2250 50  0000 L C
F 1 "OPTO_NPN" H 3850 2100 50  0000 L C
	1    3700 2200
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 49776C33
P 4700 2400
F 0 "R2" V 4780 2400 50  0000 C C
F 1 "R" V 4700 2400 50  0000 C C
	1    4700 2400
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 49776C14
P 4700 2000
F 0 "R1" V 4780 2000 50  0000 C C
F 1 "R" V 4700 2000 50  0000 C C
	1    4700 2000
	0    1    1    0   
$EndComp
$Comp
L CONN_3 K1
U 1 1 49776B30
P 5750 2100
F 0 "K1" V 5700 2100 50  0000 C C
F 1 "CONN_3" V 5800 2100 40  0000 C C
	1    5750 2100
	1    0    0    -1  
$EndComp
$EndSCHEMATC
