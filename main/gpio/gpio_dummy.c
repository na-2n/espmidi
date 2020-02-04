#include "gpio/gpio_compat.h"

void gpiocompat_init_pin(gpiocompat_pin_t pin, gpiocompat_mode_t mode)
{ }

void gpiocompat_set_pin(gpiocompat_pin_t pin, gpiocompat_dval_t state)
{ }

bool gpiocompat_get_pin(gpiocompat_pin_t pin)
{
    return false;
}