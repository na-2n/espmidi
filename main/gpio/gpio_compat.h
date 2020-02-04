#ifndef __GPIO_COMPAT_H__
#define __GPIO_COMPAT_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define LOW 0
#define HIGH 255

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef uint8_t gpiocompat_pin_t;

typedef enum
{
    GPIOCOMPAT_MODE_DISABLE,
    GPIOCOMPAT_MODE_OUTPUT,
    GPIOCOMPAT_MODE_INPUT
} gpiocompat_mode_t;

typedef bool gpiocompat_dval_t;

void gpiocompat_init_pin(gpiocompat_pin_t pin, gpiocompat_mode_t mode);

void gpiocompat_set_pin(gpiocompat_pin_t pin, gpiocompat_dval_t state);

bool gpiocompat_get_pin(gpiocompat_pin_t pin);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __GPIO_COMPAT_H__