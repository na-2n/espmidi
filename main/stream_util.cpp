#include "stream_util.h"

#include "byte_order.h"

namespace stream_util
{
    std::streamoff read_varint(std::istream* s, size_t &out, read_opt_t opts)
    {
        std::streampos start = s->tellg();

        uint8_t b = 0;
        out = 0;

        do
        {
            out = (out << 7) | ((b = s->get()) & 0x7F);
        }
        while (b & 0x80);

        byte_order::order_bytes(&out, opts & READ_BE ? SYS_BIG_ENDIAN : SYS_LITTLE_ENDIAN);

        return s->tellg() - start;
    }
}