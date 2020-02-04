#include "gpio/gpio_compat.h"

#include <stdio.h>

void gpiocompat_init_pin(gpiocompat_pin_t pin, gpiocompat_mode_t mode)
{
    printf("Dummy GPIO: init_pin(%u)\n", pin);
}

void gpiocompat_set_pin(gpiocompat_pin_t pin, gpiocompat_dval_t state)
{
    printf("Dummy GPIO: set_pin(%u): %u\n", state);
}

bool gpiocompat_get_pin(gpiocompat_pin_t pin)
{
    printf("Dummy GPIO: get_pin(%u)\n", pin);

    return false;
}