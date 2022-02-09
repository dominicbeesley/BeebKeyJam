#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "uart_rx.pio.h"
#include "keymap.h"

#define PIO_RX_PIN 28
#define GPIO_RELAY_PIN 21
#define GPIO_RST_PIN 22

#define GPIO_CA2_OUT_PIN 16
#define GPIO_CA2_IN_PIN 19
#define GPIO_KB_EN_PIN 17
#define GPIO_1MHZ_PIN 18

#define GPIO_PA7_OUT_PIN 7
#define GPIO_PA7_IN_PIN 20

#define SERIAL_BAUD (16000000/13/8/8)
#define KEYTIME 21000



// Serial string to to enable serial redirect on Beeb

char serialstring[] = {13,/*'*','F','X','1','5','6',',','0',',','2','5','2',13,*/ 
                          '*','F','X','8',',','8',13, 
                          '*','F','X','3',',','1',13,'\0'};


// bit map of keys pressed, index is row (in beeb bit order!), bit # is column
// a '1' indicates key is pressed                          
volatile unsigned int keymatrix[8];

void scancore(void) {
    static unsigned char col_ix = 0;
    static unsigned char row_ix = 0;
    static unsigned char col_load;
    static bool prev_1MHz;
    static bool now_1MHz;
    static bool ca2_o;
    static bool pa7_o = 0;
    while (1) 
    {     


        prev_1MHz = now_1MHz;
        now_1MHz = gpio_get(GPIO_1MHZ_PIN);

        if (prev_1MHz && !now_1MHz) {
            bool en = gpio_get(GPIO_KB_EN_PIN);
            gpio_put(PICO_DEFAULT_LED_PIN, !en);
            gpio_set_dir(GPIO_PA7_OUT_PIN, !en);        //only enable PA7 out when KB EN is low
            if (en) {
                //keyboard scan enabled, run through the columns
                col_ix++;
                col_ix = col_ix & 0x0F;
            } else {
                col_ix = (gpio_get(3)?1:0) |
                         (gpio_get(4)?2:0) |
                         (gpio_get(5)?4:0) |
                         (gpio_get(6)?8:0);
                row_ix = (gpio_get(0)?1:0) |
                         (gpio_get(1)?2:0) |
                         (gpio_get(2)?4:0);
            }           

            //do CA2
            ca2_o = 0;
            for (int i = 0; i < 8; i++) {
                ca2_o |= (keymatrix[i] & (1 << col_ix));
            }
            pa7_o = keymatrix[row_ix] & (1 << col_ix);      

        }


        gpio_put(GPIO_CA2_OUT_PIN, ca2_o || gpio_get(GPIO_CA2_IN_PIN));
        gpio_put(GPIO_PA7_OUT_PIN, pa7_o || gpio_get(GPIO_PA7_IN_PIN));


    }
}


 // Set up the state machine we're going to use to receive .
    PIO pio = pio0;
    uint sm = 0;

void serialrx_task()
{
/*    if (!uart_rx_program_empty(pio,sm))
        { 
            char c = uart_rx_program_getc(pio, sm) ^ 255;
            // remap return data e.g. £ ( 0x60)
            if (c==0x60)
                {
                    putchar(0xC2);putchar(0xa2);return;
                }
            putchar(c);
        }
        */
    return;
}

void key_clear(void) {
    for (int i = 0; i < 8; i++) {
        keymatrix[i] = 0;
    }
}

void key_down(unsigned int key) {

    if (key & CTRL) {
        keymatrix[0] |= 2;
    }
    if (key & SHIFT) {
        keymatrix[0] |= 1;
    }

    keymatrix[0x7 - (key & 0x7)] |= 1<<((key&0xF0) >> 4);
}

void key_up(unsigned int key) {
    if (key & ALLUP) {
        key_clear();
        return;
    }

    if (key & CTRL) {
        keymatrix[0] &= ~2;
    }
    if (key & SHIFT) {
        keymatrix[0] &= ~1;
    }

    keymatrix[0x7 - (key & 0x7)] &= ~(1<<((key&0xF0) >> 4));
}


void key_task()
{
    static int state = 0;
    static uint64_t timeout;
    static int powerstate =0;
    static int strptr = 0;

    switch (state)
    {        
    case 0: { int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT)
        { 
            if (c > 32 && c < 128) {
                putchar(c);
            } else {
                printf("%%%02XX", c);
            }
            if ( c == 0x1b )
            {
                c = getchar_timeout_us(0);
                if (c== PICO_ERROR_TIMEOUT)
                    c = 0x1b; // ESCAPE
                else 
                {   
                    puts("ESCAPE^[");
                    unsigned int tmpkey = -1;
                    // Alt A or Alt Z for cap/shift lock
                    if ((c == 'A') ||( c=='a')) tmpkey = C0+R3;// CAPS lock
                    if ((c == 'Z') ||( c=='z')) tmpkey = C0+R2;// SHIFT lock
                      
                    // multichar escape sequence
                    if ( c== 0x4f)
                    {   
                        switch (getchar_timeout_us(0))
                        {
                            case 0x41: tmpkey = C9+R4;break; // Ctrl Up
                            case 0x42: tmpkey = C9+R5;break; // Ctrl Down
                            case 0x43: tmpkey = C9+R0;break; // Ctrl Right
                            case 0x44: tmpkey = C9+R6;break; // CTRL Left
                            case 0x70: tmpkey = C10+R1;break; // Num 0
                            case 0x71: tmpkey = C11+R1;break; // Num 1
                            case 0x72: tmpkey = C12+R0;break; // Num 2
                            case 0x73: tmpkey = C12+R1;break; // Num 3
                            case 0x74: tmpkey = C10+R0;break; // Num 4
                            case 0x75: tmpkey = C11+R0;break; // Num 5
                            case 0x76: tmpkey = C10+R6;break; // Num 6
                            case 0x77: tmpkey = C11+R6;break; // Num 7
                            case 0x78: tmpkey = C10+R5;break; // Num 8   
                            case 0x79: tmpkey = C11+R5;break; // Num 9
                        }
                    }
                    if ( c== 0x5b)
                    {
                        c = getchar_timeout_us(0);
                        printf("===%02X===",c);
                        if (( c>= 0x40) && ( c<= 0x7b))
                        {
                            tmpkey = map1[c+128-0x40];
                        }
                        if (tmpkey & BREAK)
                        {
                            gpio_put(GPIO_RST_PIN,1);                   // RST is positive!
                            timeout = time_us_64()+(KEYTIME*10);
                            state = 2;
                            puts("S=2");
                            break;
                        }  
                        if ( tmpkey & MAGIC)
                        {

///? ask dp11 what this is meant to do///                            // F11
///? ask dp11 what this is meant to do///                            if (tmpkey & CTRL)
///? ask dp11 what this is meant to do///                            {
///? ask dp11 what this is meant to do///                                powerstate ^= 1;
///? ask dp11 what this is meant to do///                                gpio_put(GPIO_RST_PIN,powerstate);
///? ask dp11 what this is meant to do///                                state = 0;
///? ask dp11 what this is meant to do///                                break;
///? ask dp11 what this is meant to do///                            }

                            state = 3; 
                            puts("S=3");
                            strptr = 0;
                            break;
                        }      
                    }
                    if (tmpkey != -1)
                    {
                        key_down(tmpkey);
                        state = 1;
                        puts("S=1");
                        timeout = time_us_64()+KEYTIME;   
                        break;
                    }
                }
            } 
            if ((c == 0xc2) && (getchar_timeout_us(0)==0xa3)) c = 0x60; // £ sign
            if ( c <= 0x7f ) {
                key_down(map1[c]);
                state = 1;
                puts("S=1");
                timeout = time_us_64()+KEYTIME;   
            }
            else
                printf(" unknown char %x %x %x",c,getchar_timeout_us(0),getchar_timeout_us(0)); 
            }
        break;
        } 
    case 1: {
        if (time_us_64()>timeout) {
               key_up(ALLUP);
               state = 0;
               puts("s=0");
        }
        break;
        }
    case 2: 
        if (time_us_64()>timeout) {
            gpio_put(GPIO_RST_PIN,0);
            timeout = time_us_64()+(KEYTIME*10);
            state = 1;
            puts("s=1");
        }
        break;
    case 3:
        // F11 *FX string.
        if (time_us_64()>timeout) {
            char c = serialstring[strptr++];
            if (!c)
                {
                    key_up(ALLUP);
                    state = 0;
                    puts("s=0");
                    break;
                } 
            key_down(map1[c]);
            timeout = time_us_64()+(KEYTIME*2);       
        }
        break;
    default:
        puts("XXX");
        state = 0;
    }
}

int main()
{
    key_clear();
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

    //PA4-6, PA0-3
    for (int i = 0; i <= 6; i++) {
        gpio_init(i);
        gpio_set_pulls(i,0,1);
    }

    key_down(0);

    multicore_launch_core1(scancore);

 // Set up the state machine we're going to use to receive .
    uint offset = pio_add_program(pio, &uart_rx_program);
    uart_rx_program_init(pio, sm, offset, PIO_RX_PIN, SERIAL_BAUD);

    puts("init...");


/*
// test program for finding keycodes 
    while ( 1) {
        int c = getchar_timeout_us(100);
        if (c != PICO_ERROR_TIMEOUT)
            printf(" %x ",c&0xFF);

    }
*/

// test program to check rx uart ( inverted )
    while (true) {
        // Echo characters received from PIO to the console
        //serialrx_task();
        key_task();   
    }

    return 0;
}
/* Keyboard 

putty default
PC   BBC
F10  R0
F12  break
end  COPY

CTRL+HOME 


1a break ( Ctrl Z )

1b 1a alt break

1b escape

1b 5b 41 up
1b 5b 42 down
1b 5b 43 right
1b 5b 44 left

1b 5b 31 7e home
1b 5b 32 7e insert
1b 5b 33 7e Delete
1b 5b 34 7e end
1b 5b 35 7e pgup
1b 5b 36 7e pgdown

1b 5b 31 31 7e  F1
1b 5b 31 32 7e  F2

1b 5b 31 37 7e  F6

1b 5b 31 39 7e  F8
1b 5b 32 30 7e  F9
1b 5b 32 31 7e  F10
1b 5b 32 33 7e  F11


1b 5b 32 33 7e Shift F1
1b 5b 32 39 7e Shift F6

1b 1b 5b 31 31 7e  Alt F1
1b 1b 5b 31 32 7e  ALt F2


SCO

1b 5b 4d   F1
1b 5b 4e

1b 5b 59 shift F1

1b 5b 6b CTRL F1

1b 5b 77 SHIFT+CTRL F1 

1b 5b 41 up
1b 5b 42 down
1b 5b 43 right
1b 5b 44 left
1b 5b 46 end
1b 5b 47 page down
1b 5b 48 home
1b 5b 49 page up
1b 5b 4c insert

1b 5b 5a shift tab


1b 4f 41 CTRL  up
1b 4f 42 CTRL down
1b 4f 43 CTRL right
1b 4f 44 CTRL left

numeric keypad
( application mode)

1b 4f 70 0
1b 4f 71 1


*/


