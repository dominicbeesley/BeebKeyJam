#ifndef __KEYMAP_H__
#define __KEYMAP_H__

//NOTE: the orders and sense of these is quite different to DP11's masterkbd
//for ease of conversion I have kept the row order (opposite to the BIT order
//on the 74LS251

// keyboard mapping

#define MAGIC (1<<27)
#define BREAK (1<<26)
#define CTRL  (1<<25)
#define SHIFT (1<<24)
#define ALLUP (1<<28)


#define R7 0x07
#define R6 0x06
#define R5 0x05
#define R4 0x04
#define R3 0x03
#define R2 0x02
#define R1 0x01
#define R0 0x00

#define C0  0x00
#define C1  0x10
#define C2  0x20
#define C3  0x30
#define C4  0x40
#define C5  0x50
#define C6  0x60
#define C7  0x70
#define C8  0x80
#define C9  0x90
#define C10 0xA0
#define C11 0xB0
#define C12 0xC0

extern unsigned int map1[];

#endif