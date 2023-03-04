#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"

const uint32_t LED_PIN = 10;

int main() {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    printf("Hello!\n");
    while (1) {
        gpio_put(LED_PIN, 0);
        sleep_ms(2000);
        gpio_put(LED_PIN, 1);
        sleep_ms(2000);
    }
}