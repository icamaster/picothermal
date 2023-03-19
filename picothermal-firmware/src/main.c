/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "version.h"


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;
#define LED_PIN          10
#define SW_PIN           11

#define LEPTON_MOSI      3
#define LEPTON_MISO      0
#define LEPTON_SCLK      2
#define LEPTON_CS        1

#define LEPTON_SDA       6
#define LEPTON_SCL       7

#define LEPTON_VSYNC_PIN 8
#define LEPTON_RESET_PIN 24
#define LEPTON_PWRDN_PIN 25

void led_blinking_task(void);
void video_task(void);

static uint64_t board_millis() { return to_us_since_boot(get_absolute_time()); }

/*------------- MAIN -------------*/
int main(void) {
    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(SW_PIN, GPIO_IN);

    gpio_set_dir(LEPTON_MOSI, GPIO_OUT);
    gpio_set_dir(LEPTON_MISO, GPIO_IN);
    gpio_set_dir(LEPTON_SCLK, GPIO_OUT);
    gpio_set_dir(LEPTON_CS, GPIO_OUT);
    gpio_set_dir(LEPTON_SDA, GPIO_IN);
    gpio_set_dir(LEPTON_SCL, GPIO_IN);
    gpio_set_dir(LEPTON_VSYNC_PIN, GPIO_IN);

    gpio_put(LEPTON_RESET_PIN, 1);
    gpio_put(LEPTON_PWRDN_PIN, 1);

    printf("Version: %d\n", PICOTHERMAL_VERSION);

    while (1) {
        tud_task();  // tinyusb device task
        led_blinking_task();

        video_task();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) { blink_interval_ms = BLINK_MOUNTED; }

// Invoked when device is unmounted
void tud_umount_cb(void) { blink_interval_ms = BLINK_NOT_MOUNTED; }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) { blink_interval_ms = BLINK_MOUNTED; }

//--------------------------------------------------------------------+
// USB Video
//--------------------------------------------------------------------+
static unsigned frame_num = 0;
static unsigned tx_busy = 0;
static unsigned interval_ms = 1000 / FRAME_RATE;

static uint8_t frame_buffer[FRAME_WIDTH * FRAME_HEIGHT * 16 / 8];
static void fill_color_bar(uint8_t *buffer, unsigned start_position) {
    /* EBU color bars
     * See also https://stackoverflow.com/questions/6939422 */
    static uint8_t const bar_color[8][4] = {
        /*  Y,   U,   Y,   V */
        {235, 128, 235, 128}, /* 100% White */
        {219, 16, 219, 138},  /* Yellow */
        {188, 154, 188, 16},  /* Cyan */
        {173, 42, 173, 26},   /* Green */
        {78, 214, 78, 230},   /* Magenta */
        {63, 102, 63, 240},   /* Red */
        {32, 240, 32, 118},   /* Blue */
        {16, 128, 16, 128},   /* Black */
    };
    uint8_t *p;

    /* Generate the 1st line */
    const uint8_t *end = &buffer[FRAME_WIDTH * 2];
    unsigned idx = (FRAME_WIDTH / 2 - 1) - (start_position % (FRAME_WIDTH / 2));
    p = &buffer[idx * 4];
    for (unsigned i = 0; i < 8; ++i) {
        for (int j = 0; j < FRAME_WIDTH / (2 * 8); ++j) {
            memcpy(p, &bar_color[i], 4);
            p += 4;
            if (end <= p) {
                p = buffer;
            }
        }
    }
    /* Duplicate the 1st line to the others */
    p = &buffer[FRAME_WIDTH * 2];
    for (unsigned i = 1; i < FRAME_HEIGHT; ++i) {
        memcpy(p, buffer, FRAME_WIDTH * 2);
        p += FRAME_WIDTH * 2;
    }
}

void video_task(void) {
    static unsigned start_ms = 0;
    static unsigned already_sent = 0;

    if (!tud_video_n_streaming(0, 0)) {
        already_sent = 0;
        frame_num = 0;
        return;
    }

    if (!already_sent) {
        already_sent = 1;
        start_ms = board_millis();
        fill_color_bar(frame_buffer, frame_num);
        tud_video_n_frame_xfer(0, 0, (void *)frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
    }

    unsigned cur = board_millis();
    if (cur - start_ms < interval_ms) return;  // not enough time
    if (tx_busy) return;
    start_ms += interval_ms;

    fill_color_bar(frame_buffer, frame_num);
    tud_video_n_frame_xfer(0, 0, (void *)frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
}

void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) {
    (void)ctl_idx;
    (void)stm_idx;
    tx_busy = 0;
    /* flip buffer */
    ++frame_num;
}

int tud_video_commit_cb(
    uint_fast8_t ctl_idx, uint_fast8_t stm_idx, video_probe_and_commit_control_t const *parameters) {
    (void)ctl_idx;
    (void)stm_idx;
    /* convert unit to ms from 100 ns */
    interval_ms = parameters->dwFrameInterval / 10000;
    return VIDEO_ERROR_NONE;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
    static uint32_t start_ms = 0;

    // Blink every interval ms
    if (board_millis() - start_ms < blink_interval_ms) return;  // not enough time
    start_ms += blink_interval_ms;

    gpio_put(LED_PIN, !gpio_get(LED_PIN));  // toggle
}
