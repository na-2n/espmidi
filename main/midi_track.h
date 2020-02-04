#ifndef __MIDI_TRACK_H__
#define __MIDI_TRACK_H__

#include "preproc.h"

#include "midi_util.h"
#include "midi_const.h"
#include "midi_event.h"
#include "stream_util.h"

#include <cstdint>
#include <cstddef>
#include <istream>

namespace midi
{
    class midi_track
    {
        friend class midi_file;

    private:
        char TAG[18] = "midi_track[?]";

        std::istream* _stream;
        std::streampos _start_pos = 0;
        std::streamoff _read_offset = 0;

        uint32_t _length = 0;
        uint16_t _track_id = 0;

        size_t _tick_count = 0;
        uint8_t _ev_byte = 0;
        uint8_t _last_ev_byte = 0;
        
        inline void _s_read(void* buf, std::streamsize len)
        {
            _stream->read(reinterpret_cast<char*>(buf), len);

            _read_offset += len;
        }

        inline void _s_ignore(std::streamsize len)
        {
            _stream->ignore(len);

            _read_offset += len;
        }

        inline uint8_t _s_get()
        {
            _read_offset++;

            return _stream->get();
        }

        inline std::streamoff _s_u_read_varint(size_t &out, stream_util::read_opt_t opt = stream_util::_NONE)
        {
            std::streamoff ret = stream_util::read_varint(_stream, out, opt);

            _read_offset += ret;

            return ret;
        }

        HANDLER_FIELD(midi);
        HANDLER_FIELD(sysex);
        HANDLER_FIELD(meta);

        TRIGGER_FUNC(midi);
        TRIGGER_FUNC(sysex);
        TRIGGER_FUNC(meta);

        void _parse_meta_ev();
        void _parse_sysex_ev();
        void _parse_midi_ev();

    public:
        midi_track(std::istream* stream)
            : _stream{stream}
        { }

        uint32_t load_chunk(uint16_t track_id);

        bool next_event(size_t ticks);

        uint32_t length() CONST_RETURN(_length);

        uint16_t id() CONST_RETURN(_track_id);

        HANDLER_FUNC(midi);
        HANDLER_FUNC(sysex);
        HANDLER_FUNC(meta);
    };
}

#endif // __MIDI_TRACK_H__