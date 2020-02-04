#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum
{
    SYS_LITTLE_ENDIAN = 0x03020100,
    SYS_BIG_ENDIAN    = 0x00010203
} byte_order_t;

static const union
{
    uint8_t buf[4];
    uint32_t value;
} _sys_byte_order = { { 0, 1, 2, 3 } };

inline uint8_t bitrev(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;

    return b;
}

inline uint8_t bitinv(uint8_t b)
{
    return b ^ 0xFF;
}

#define SYS_BYTE_ORDER (byte_order_t)_sys_byte_order.value

#ifdef __cplusplus
}
#endif // __cplusplus

// ----------------------------------------- //

#ifdef __cplusplus

#include <cstddef>
#include <algorithm>

namespace byte_order
{
    template<typename T, size_t s = sizeof(T)>
    void swap_bytes(T* val)
    {
        char* buf = reinterpret_cast<char*>(val);

        for (size_t h = s-1, l = 0; h > l; l++, h--)
        {
            std::swap(buf[l], buf[h]);
        }
    }

    template<typename T, size_t s = sizeof(T)>
    void order_bytes(T* val, byte_order_t order)
    {
        if ((SYS_BYTE_ORDER == SYS_LITTLE_ENDIAN && (order == SYS_BIG_ENDIAN)) ||
            (SYS_BYTE_ORDER == SYS_BIG_ENDIAN    && (order == SYS_LITTLE_ENDIAN)))
            byte_order::swap_bytes<T, s>(val);
    }
}

#endif // __cplusplus

#endif // __BYTE_ORDER_H__