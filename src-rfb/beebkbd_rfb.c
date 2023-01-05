#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/clocks.h"
#include "rgb_cap.pio.h"


const uint CAPTURE_PIN_SYNC = 17;
const uint CAPTURE_PIN_BASE = 15;
const uint CAPTURE_PIN_COUNT = 2;
const uint CAPTURE_N_SAMPLES = 800*200;

uint buf_size_words;

static inline uint bits_packed_per_word(uint pin_count) {
    // If the number of pins to be sampled divides the shift register size, we
    // can use the full SR and FIFO width, and push when the input shift count
    // exactly reaches 32. If not, we have to push earlier, so we use the FIFO
    // a little less efficiently.
    const uint SHIFT_REG_WIDTH = 32;
    return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

void rgb_cap_init(PIO pio, uint sm, uint pin_base, uint pin_count) {

    uint offset = pio_add_program(pio, &rgb_cap_program);

    pio_sm_config c = rgb_cap_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_base); // for WAIT, IN
    // Shift to right, autopush enabled
    // SM transmits 1 pixel per 2 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (2 * 16000000);
    sm_config_set_clkdiv(&c, div);
    
    // Note that we may push at a < 32 bit threshold if pin_count does not
    // divide 32. We are using shift-to-right, so the sample data ends up
    // left-justified in the FIFO in this case, with some zeroes at the LSBs.
    sm_config_set_in_shift(&c, true, true, bits_packed_per_word(pin_count));
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE);
    pio_sm_init(pio, sm, offset, &c);

}

void rgb_cap_arm(PIO pio, uint sm, uint dma_chan, uint32_t *capture_buf, size_t capture_size_words,
                        uint trigger_pin, bool trigger_level) {
    pio_sm_set_enabled(pio, sm, false);
    // Need to clear _input shift counter_, as well as FIFO, because there may be
    // partial ISR contents left over from a previous run. sm_restart does this.
    pio_sm_clear_fifos(pio, sm);
    pio_sm_put_blocking(pio, sm, (800 << 16) | (200));
    pio_sm_restart(pio, sm);

    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));

    dma_channel_configure(dma_chan, &c,
        capture_buf,        // Destination pointer
        &pio->rxf[sm],      // Source pointer
        capture_size_words, // Number of transfers
        true                // Start immediately
    );

    //pio_sm_exec(pio, sm, pio_encode_wait_gpio(trigger_level, trigger_pin));
    pio_sm_set_enabled(pio, sm, true);
}

void outout(const void *p, int l) {
    const char * pp = p;
    while (l-- > 0) {
        putchar_raw(*(pp++));
    }
}

void print_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
//    // Display the capture buffer in text form, like this:
//    // 00: __--__--__--__--__--__--
//    // 01: ____----____----____----
//    printf("Capture:\n");
//    // Each FIFO record may be only partially filled with bits, depending on
//    // whether pin_count is a factor of 32.
//    uint record_size_bits = bits_packed_per_word(pin_count);
//    for (int pin = 0; pin < pin_count; ++pin) {
//        printf("%02d: ", pin + pin_base);
//        for (int sample = 0; sample < n_samples; ++sample) {
//            uint bit_index = pin + sample * pin_count;
//            uint word_index = bit_index / record_size_bits;
//            // Data is left-justified in each FIFO entry, hence the (32 - record_size_bits) offset
//            uint word_mask = 1u << (bit_index % record_size_bits + 32 - record_size_bits);
//            printf(buf[word_index] & word_mask ? "-" : "_");
//        }
//        printf("\n");
//    }

        uint32_t marker = 0xFFFFFFFF;
        outout(&marker, 4);
        outout(buf, buf_size_words * sizeof(uint32_t));
}

int main() {
    stdio_init_all();

    for (uint i = CAPTURE_PIN_BASE; i < CAPTURE_PIN_BASE + CAPTURE_PIN_COUNT; i++) {
        gpio_init(i);
        gpio_set_pulls(i,0,1);
    }
    gpio_init(CAPTURE_PIN_SYNC);
    gpio_set_pulls(CAPTURE_PIN_SYNC,0,1);


    getchar();

    printf("PIO logic analyser example\n");

    getchar();

    // We're going to capture into a u32 buffer, for best DMA efficiency. Need
    // to be careful of rounding in case the number of pins being sampled
    // isn't a power of 2.
    uint total_sample_bits = CAPTURE_N_SAMPLES * CAPTURE_PIN_COUNT;
    total_sample_bits += bits_packed_per_word(CAPTURE_PIN_COUNT) - 1;
    buf_size_words = total_sample_bits / bits_packed_per_word(CAPTURE_PIN_COUNT);
    uint32_t *capture_buf = malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);

    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    PIO pio = pio0;
    uint sm = 0;
    uint dma_chan = 0;

    rgb_cap_init(pio, sm, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT);

    while(1) {

        getchar();

        printf("Arming trigger\n");
        rgb_cap_arm(pio, sm, dma_chan, capture_buf, buf_size_words, CAPTURE_PIN_SYNC, false);


        // The logic analyser should have started capturing as soon as it saw the
        // first transition. Wait until the last sample comes in from the DMA.
        dma_channel_wait_for_finish_blocking(dma_chan);


        print_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES);

    }


    while (1) {
        printf("BOO\n");
    }
}
