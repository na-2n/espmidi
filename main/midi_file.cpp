#include "midi_file.h"

#include "log.h"
#include "byte_order.h"
#include "midi_ev_id.h"

#include <chrono>

#define _PROXY_1(type, ev) do              \
{                                          \
    if (_playback != nullptr)              \
        _playback->_on_##type##_event(ev); \
    else LOGE(TAG, "_playback == nullptr");\
}                                          \
while (0)

#define _PROXY_2(type) _PROXY_1(type, ev)

#define PROXY_PLAYBACK(...) _GET_ARG3(__VA_ARGS__, _PROXY_1, _PROXY_2)(__VA_ARGS__)

static const char* TAG = "midi_file";

namespace midi
{
    inline uint64_t micros_now()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count() * 1000;
    }

    uint32_t midi_file::_calc_ticks()
    {
        uint32_t ticks = 0;
        uint64_t time_now = micros_now();
        uint64_t elapsed = ((_tick_offset + time_now) - _last_tick_calc);

        if (elapsed >= _tick_time)
        {
            ticks = (elapsed / _tick_time);
            _tick_offset = (elapsed - (ticks * _tick_time));
            _last_tick_calc = time_now;
        }

        return ticks;
    }

    void midi_file::load()
    {
        if (_stream == nullptr)
            return;

        char chunk_id[5] = {0};

        _stream->read(chunk_id, 4);

        if (strcmp(chunk_id, HEADER_CHUNK_ID))
        {
            // invalid chunk id

            LOGE(TAG, "Invalid chunk ID");
            return;
        }

        _stream->ignore(4); // header chunk length is always 6

        _stream->read(reinterpret_cast<char*>(&_hdr), 6);

        byte_order::order_bytes(&_hdr.tracks, SYS_BIG_ENDIAN);
        byte_order::order_bytes(&_hdr.timediv, SYS_BIG_ENDIAN);

        if (_hdr.timediv & 0x8000) // timecode
        {
            // ms res = (1000ms / (fps * subres))
            // ppqn = (fps * res)

            int8_t fps = bitrev(_hdr.timediv >> 8) + 1;
            int8_t res = (_hdr.timediv & 0xFF);

            _hdr.timediv = (fps * res);
        }

        _ppqn = _hdr.timediv;

        LOGI(TAG, "PPQN = %u", _ppqn);

        _tracks = new midi_track*[_hdr.tracks];

        for (uint16_t i = 0; i < _hdr.tracks; i++)
        {
            _tracks[i] = new midi_track{_stream};

            midi_track* t = _tracks[i];

            LOGI(TAG, "Loading track %u...", i);

            uint32_t size = t->load_chunk(i);

            t->proxy_event(midi);
            t->proxy_event(sysex);
            t->proxy_event(meta);

            t->internal_event(midi);
            t->internal_event(sysex);
            t->internal_event(meta);

            _stream->seekg(size, std::istream::cur);
        }
    }

    midi_statcode_t midi_file::next_event()
    {
        if (_eot)
            return EOT;

        if (_paused)
            return PAUSED;

        if (!_tick_time)
            _update_tick_time();

        uint32_t ticks = _calc_ticks();

        if (!ticks)
            return NO_TICKS;

        for (uint16_t n = 0; n < _hdr.tracks; n++)
        {
            midi_track* t = _tracks[n];

            for (uint8_t i = 0; i < 100; i++)
            {
                if (!t->next_event(i == 0 ? ticks : 0))
                    break;
            }
        }

        return OK;
    }

    void midi_file::_internal_midi_ev(midi_event_t ev)
    {
        PROXY_PLAYBACK(midi);
    }

    void midi_file::_internal_sysex_ev(sysex_event_t ev)
    {
        PROXY_PLAYBACK(sysex);
    }

    void midi_file::_internal_meta_ev(meta_event_t ev)
    {
        PROXY_PLAYBACK(meta);

        switch (ev.type)
        {
            case META_TEMPO:
                byte_order::order_bytes<char, 3>(ev.data, SYS_BIG_ENDIAN);

                memcpy(&_mpqn, ev.data, 3);

                _tempo = calc_bpm(_mpqn);

                LOGI(TAG, "BPM = %u", _tempo);
                break;

            case META_EOT:
                _eot = true;
                break;

            default:
                break;
        }
    }
}