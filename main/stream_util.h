#ifndef __STREAM_UTIL_H__
#define __STREAM_UTIL_H__

#include <cstdint>
#include <cstddef>
#include <istream>

namespace stream_util
{
    typedef enum
    {
        _NONE,
        READ_BE = 0x01,
    } read_opt_t;

    std::streamoff read_varint(std::istream* s, size_t &out, read_opt_t opts = _NONE);
}

#endif // __STREAM_UTIL_H__