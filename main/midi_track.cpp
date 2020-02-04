#include "midi_track.h"

#include "log.h"
#include "byte_order.h"

#include <cstring>
#include <thread>
#include <chrono>

#define GOTO_OFFSET                        \
    uint32_t __origpos = _stream->tellg(); \
    _stream->seekg(_start_pos + _read_offset)

#define RETURN_OFFSET _stream->seekg(__origpos)

#define CHECK_BOUNDS             \
    if (_read_offset >= _length) \
        return false

namespace midi
{
    void midi_track::_parse_meta_ev()
    {
        meta_event_t ev;

        ev.type = _s_get();

        _s_u_read_varint(ev.length);

        ev.data = new char[ev.length];

        _s_read(ev.data, ev.length);

        _trigger_meta_ev(ev);
    }

    void midi_track::_parse_sysex_ev()
    {
        sysex_event_t ev;

        _s_u_read_varint(ev.length);

        ev.data = new char[ev.length];

        _s_read(ev.data, ev.length);

        _trigger_sysex_ev(ev);
    }

    void midi_track::_parse_midi_ev()
    {
        midi_event_t ev;

        if (!(_ev_byte & 0x80))
        {
            *ev.params = _ev_byte;
            _ev_byte = _last_ev_byte;
        }
        else
        {
            *ev.params = _s_get();
            _last_ev_byte = _ev_byte;                        
        }

        ev.type = (_ev_byte >> 4);
        ev.channel = (_ev_byte & 0xF);

        if (ev.type >= 0x8 && ev.type <= 0xE)
        {
            if (ev.type != 0xC && ev.type != 0xD)
                ev.params[1] = _s_get();

            _trigger_midi_ev(ev);
        }
    }

    uint32_t midi_track::load_chunk(uint16_t track_id)
    {
        _track_id = track_id;

        sprintf(TAG, "midi_track[%u]", track_id);

        _start_pos = _stream->tellg();

        GOTO_OFFSET;
        
        char chunk_id[5] = {0};

        _s_read(chunk_id, 4);

        if (strcmp(chunk_id, TRACK_CHUNK_ID))
        {
            LOGE(TAG, "Invalid chunk ID (%s)", chunk_id);
            return 0;
        }

        _s_read(&_length, 4);

        byte_order::order_bytes(&_length, SYS_BIG_ENDIAN);

        LOGI(TAG, "chunk = '%s'", chunk_id);
        LOGI(TAG, "length = %u", _length);

        RETURN_OFFSET;

        return _length + 8;
    }

    bool midi_track::next_event(size_t ticks)
    {
        GOTO_OFFSET;

        CHECK_BOUNDS;

        size_t delta;

        std::streamoff read_size = stream_util::read_varint(_stream, delta);

        _tick_count += ticks;

        if (_tick_count < delta)
            return false;

        _tick_count -= delta;

        _read_offset += read_size;

        _ev_byte = _s_get();

        if (_ev_byte == 0xFF) _parse_meta_ev();
        else if (_ev_byte == 0xF7 || _ev_byte == 0xF0) _parse_sysex_ev();
        else _parse_midi_ev();

        RETURN_OFFSET;

        return true;
    }
}
