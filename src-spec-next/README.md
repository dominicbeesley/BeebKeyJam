# Spectrum Next PS/2 keyboard

This folder contains a version of the firmware to support connecting a BBC Micro keyboard to a Spectrum Next for use with Hoglets BeebFPGA Core.

# Connections

The beeb keyboard should be connected to J2 - be careful to get the orientation correct pin 1 (next to J2 designator) should connect to the left most pin of the keyboard

A PS/2 cable should be connected as follows - be careful sending 5V up the wrong pin could damage you Spec Next, Pico, Keyboard!

                          o===o===o==============  PS/2 - CLK - pin 5 / green
                          |   |  _|_ 
                          |  _|_ | |  
		+---+            _|_ | | |R| 
	17	| O |            \ / |R| |_|
		| O |            --- |_|  |
	15	| O | +5V       ==|===|===o==============  PS/2 - +5V - pin 5 / red
		| O | CA2_o_5   ==o===o
		| O |
		| O | PA7_o_5	========================== PS/2 - DAT - pin 1 / grey
		| O |
	10	| O |
		| O |
		| O |
		| O |
		| O |
	5	| O |
		| O |
		| O |
		| O | RESET		====o--[ R   ]--o========= J4 pin #5
	1	| O | GND		========================== PS/2 - GND - pin 3 / blue
		+---+
		J1

All R's are 4k7, diode is 1n4148 or small signal schottky

The R between +5V and clock is optional, it is merely there to assist when debugging without any
PS/2 host plugged in and provides a pull-up.

