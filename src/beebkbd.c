#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "uart_rx.pio.h"
#include "keymap.h"

/*

    __  __           __  ___________                __      __
   / / / /___  _____/ /_/ ____/ ___/   ____  ____  / /_  __/ /
  / /_/ / __ \/ ___/ __/ /_   \__ \   / __ \/ __ \/ / / / / /
 / __  / /_/ (__  ) /_/ __/  ___/ /  / /_/ / / / / / /_/ /_/
/_/ /_/\____/____/\__/_/    /____/   \____/_/ /_/_/\__, (_)
                                                  /____/

This version passes USB-serial data to / from the host via hidden keyboard columns
Column F: Status: Bit 7 - RXF - char pending, Bit 6 - TXE - can transmit
Column E: RXD pending receive byte, cleared after column E deselected to "ACK"
Column C,D: TXD (see below)


Data is sent from BBC to host by:

bit 2..0 of the row address are shifted into a transmit register each time
column transitions D->C, data is transmitted when "ACK" selected

Note: this build does no keyboard jamming but DOES:
 - passes CA2 through from keyboard only when column <=9 or keyboard not "enabled"
 - passed through PA7 for columns <= 9

"ACK" condidition is when rowix = 0, colix = A

*/


#define PIO_RX_PIN 28

#define GPIO_RELAY_PIN      17
#define GPIO_RST_PIN        15

#define GPIO_CA2_OUT_PIN    16
#define GPIO_CA2_IN_PIN     4
#define GPIO_KB_EN_PIN      13
#define GPIO_1MHZ_PIN       14

#define GPIO_PA7_OUT_PIN    22
#define GPIO_PA7_IN_PIN     28

//row
#define GPIO_PA4_IN_PIN     12
#define GPIO_PA5_IN_PIN     11
#define GPIO_PA6_IN_PIN     10

//col
#define GPIO_PA0_IN_PIN     9
#define GPIO_PA1_IN_PIN     8
#define GPIO_PA2_IN_PIN     7
#define GPIO_PA3_IN_PIN     6

// a lookup table to convert PA to col and ix
const unsigned char col_lk[] = {
    0x0, 0x8, 0x4, 0xC,
    0x2, 0xA, 0x6, 0xE,
    0x1, 0x9, 0x5, 0xD,
    0x3, 0xB, 0x7, 0xF
    };

const unsigned char row_lk[] = {
    0x0, 0x4, 0x2, 0x6,
    0x1, 0x5, 0x3, 0x7
    };

//unused so far
#define GPIO_LED1_PIN       3
#define GPIO_LED2_PIN       2
#define GPIO_LED3_PIN       5

#define GPIO_TEST           18  


#define SERIAL_BAUD (16000000/13/8/8)
#define KEYTIME 21000


// bit map of keys pressed, index is row (in beeb bit order!), bit # is column
// a '1' indicates key is pressed                          
volatile unsigned int keymatrix[8];

volatile unsigned char tx_char;     // set in scancore
volatile bool txf;                  // set in scancore
volatile bool txf_ack;              // set in key_task

volatile unsigned char rx_char;     // set in key_task
volatile bool rxf;                  // set in key_task
volatile bool rxf_ack;              // set in scancore


void scancore(void) {
    static unsigned char prev_col_ix = 0;
    static unsigned char prev_row_ix = 0;
    static unsigned char prev_prev_col_ix = 0;
    static unsigned char prev_prev_row_ix = 0;
    static unsigned char col_ix = 0;
    static unsigned char row_ix = 0;
    static unsigned char col_load;
    static bool prev_1MHz;
    static bool now_1MHz;
    static bool ca2_o = 0;
    static bool pa7_o = 0;
    static uint32_t pins;
    while (1) 
    {     


        prev_1MHz = now_1MHz;
        pins = gpio_get_all();
        now_1MHz = pins & (1 << GPIO_1MHZ_PIN);


        if (!prev_1MHz && now_1MHz) {
            bool en = pins & (1 << GPIO_KB_EN_PIN);
            gpio_set_dir(GPIO_PA7_OUT_PIN, !en);        //only enable PA7 out when KB EN is low
            if (en) {
                //keyboard scan enabled, run through the columns
                col_ix++;
                col_ix = col_ix & 0x0F;

                prev_col_ix = -1;
                prev_row_ix = -1;
                prev_prev_col_ix = -1;
                prev_prev_row_ix = -1;

            } else {

                col_ix = col_lk[(pins >> GPIO_PA3_IN_PIN) & 0xF];
                row_ix = row_lk[(pins >> GPIO_PA6_IN_PIN) & 0x7];
//                col_ix = (gpio_get(GPIO_PA0_IN_PIN)?1:0) |
//                         (gpio_get(GPIO_PA1_IN_PIN)?2:0) |
//                         (gpio_get(GPIO_PA2_IN_PIN)?4:0) |
//                         (gpio_get(GPIO_PA3_IN_PIN)?8:0);                
//                row_ix = (gpio_get(GPIO_PA4_IN_PIN)?1:0) |
//                         (gpio_get(GPIO_PA5_IN_PIN)?2:0) |
//                         (gpio_get(GPIO_PA6_IN_PIN)?4:0);

                pa7_o = false;
                if (
                       prev_col_ix == col_ix 
                    && prev_row_ix == row_ix
                    ) {
                    if (col_ix == 0xC) {
                        if (prev_prev_col_ix == 0xD) {
                            tx_char = (unsigned char)((tx_char >> 3) | (row_ix << 5));
                        }
                    } else if (col_ix == 0xA && row_ix == 0x0) {
                        if (prev_prev_col_ix == 0x0C) {
                            txf = !txf_ack;
                            pa7_o = true;
                        } else if (prev_prev_col_ix == 0xE) {
                            rxf_ack = rxf;
                            pa7_o = true;
                        }
                    } 
                    prev_prev_col_ix = col_ix;
                    prev_prev_row_ix = row_ix;
                }

                if (col_ix == 0xE) {
                    pa7_o = rx_char & (1 << row_ix);                          
                } else if (col_ix == 0xF) {
                    if (row_ix == 7) 
                        pa7_o = rxf != rxf_ack;
                    else if (row_ix == 6)
                        pa7_o = txf == txf_ack;
                } 

                prev_col_ix = col_ix;
                prev_row_ix = row_ix;
            }           

            if (col_ix <= 9) {
                pa7_o = false;
//                //do CA2
//                ca2_o = 0;
//                for (int i = 0; i < 8; i++) {
//                    ca2_o |= (keymatrix[i] & (1 << col_ix));
//                }
//                pa7_o |= keymatrix[row_ix] & (1 << col_ix);      
            }
        }


        gpio_put(GPIO_PA7_OUT_PIN, pa7_o || (col_ix <= 9 && gpio_get(GPIO_PA7_IN_PIN)));
        gpio_put(GPIO_CA2_OUT_PIN, ca2_o || gpio_get(GPIO_CA2_IN_PIN));
    }
}

void keycore()
{
    while (1) {
        if (txf != txf_ack) {
            putchar_raw(tx_char);
            txf_ack = txf;
        }

        if (rxf == rxf_ack) {
            int x = getchar_timeout_us(0);
            if (x != PICO_ERROR_TIMEOUT) {
                rx_char = (unsigned char)x;
                rxf = !rxf_ack;            
            }
        }
    }
}

int main()
{
    stdio_init_all();

    //setup keyboard IOs
    for(uint i=0 ; i <(8+13); i++ )
        gpio_init(i);

    gpio_init(GPIO_CA2_OUT_PIN);
    gpio_put(GPIO_CA2_OUT_PIN, 0);
    gpio_set_dir(GPIO_CA2_OUT_PIN, 1);

    gpio_init(GPIO_RELAY_PIN);
    gpio_init(GPIO_RST_PIN);
    gpio_set_dir(GPIO_RST_PIN,1);
    gpio_put(GPIO_RST_PIN,0);

    gpio_put(GPIO_RELAY_PIN,0);

    gpio_init(GPIO_KB_EN_PIN);
    gpio_set_pulls(GPIO_KB_EN_PIN,0,1);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN,1);

    gpio_init(GPIO_1MHZ_PIN);
    gpio_set_pulls(GPIO_1MHZ_PIN,0,1);
    gpio_init(GPIO_CA2_IN_PIN);
    gpio_set_pulls(GPIO_CA2_IN_PIN,0,1);

    gpio_init(GPIO_PA7_IN_PIN);
    gpio_set_pulls(GPIO_PA7_IN_PIN,0,1);

    gpio_init(GPIO_PA7_OUT_PIN);

    gpio_init(GPIO_PA3_IN_PIN);
    gpio_set_pulls(GPIO_PA3_IN_PIN,0,1);
    gpio_init(GPIO_PA4_IN_PIN);
    gpio_set_pulls(GPIO_PA4_IN_PIN,0,1);
    gpio_init(GPIO_PA5_IN_PIN);
    gpio_set_pulls(GPIO_PA5_IN_PIN,0,1);
    gpio_init(GPIO_PA6_IN_PIN);
    gpio_set_pulls(GPIO_PA6_IN_PIN,0,1);
    gpio_init(GPIO_PA0_IN_PIN);
    gpio_set_pulls(GPIO_PA0_IN_PIN,0,1);
    gpio_init(GPIO_PA1_IN_PIN);
    gpio_set_pulls(GPIO_PA1_IN_PIN,0,1);
    gpio_init(GPIO_PA2_IN_PIN);
    gpio_set_pulls(GPIO_PA2_IN_PIN,0,1);

    gpio_init(GPIO_TEST);
    gpio_set_dir(GPIO_TEST,1);
    gpio_put(GPIO_TEST,0);


    txf = false;
    txf_ack = false;

    rxf = false;
    rxf_ack = false;

    multicore_launch_core1(scancore);

    keycore();

    return 0;
}
