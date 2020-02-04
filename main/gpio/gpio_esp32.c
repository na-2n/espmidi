#include "gpio/gpio_compat.h"

#include <driver/gpio.h>

void gpiocompat_init_pin(gpiocompat_pin_t pin, gpiocompat_mode_t mode)
{
    gpio_num_t _pin = (gpio_num_t)pin;
    gpio_mode_t _mode = (mode == GPIOCOMPAT_MODE_OUTPUT) ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;

    gpio_pad_select_gpio(_pin);
    gpio_set_direction(_pin, _mode);
}

void gpiocompat_set_pin(gpiocompat_pin_t pin, gpiocompat_dval_t state)
{
    gpio_set_level((gpio_num_t)pin, state);
}

bool gpiocompat_get_pin(gpiocompat_pin_t pin)
{
    return gpio_get_level((gpio_num_t)pin);
}