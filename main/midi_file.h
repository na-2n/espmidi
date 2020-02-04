#ifndef __MIDI_FILE_H__
#define __MIDI_FILE_H__

#include "preproc.h"

#include "midi_util.h"
#include "midi_const.h"
#include "midi_event.h"
#include "midi_track.h"
#include "midi_playback.h"

#include <cstdint>
#include <cstring>
#include <istream>

namespace midi
{
    typedef struct
    {
        uint16_t format;
        uint16_t tracks;
        uint16_t timediv;
    } header_chunk_t;

    typedef enum
    {
        PAUSED   = -2,
        NO_TICKS = -1,
        EOT      =  0, // End Of Track
        OK       =  1,
    } midi_statcode_t;

    class midi_file
    {
    private:
        std::istream* _stream;
        midi_playback* _playback;
        midi_track** _tracks;

        header_chunk_t _hdr;

        bool _eot = false;
        bool _paused = false;

        uint64_t _last_tick_calc = 0;
        uint64_t _tick_offset = 0;
        uint32_t _tick_time = 0;

        uint32_t _mpqn = 500000; // 60,000,000 / 500,000 = 120bpm
        uint32_t _tempo_offset = 0;
        uint32_t _tempo = DEFAULT_TEMPO;
        uint16_t _ppqn = DEFAULT_PPQN;

        uint32_t _calc_ticks();
        
        inline void _update_tick_time()
        {
            _tick_time = (MSPM / (_tempo + _tempo_offset)) / _ppqn;
        }

        INTERNAL_EV_FUNC(midi);
        INTERNAL_EV_FUNC(sysex);
        INTERNAL_EV_FUNC(meta);

        HANDLER_FIELD(midi);
        HANDLER_FIELD(sysex);
        HANDLER_FIELD(meta);

        TRIGGER_FUNC(midi);
        TRIGGER_FUNC(sysex);
        TRIGGER_FUNC(meta);

    public:
        midi_file()
        { }

        midi_file(std::istream* stream) : _stream{stream}
        { }

        ~midi_file()
        {
            if (_tracks != nullptr)
            {
                delete[] _tracks;
            }
        }

        void load();

        midi_statcode_t next_event();

        void use_stream(std::istream* stream)
        {
            _stream = stream;
        }

        void use_playback(midi_playback* playback)
        {
            _playback = playback;
        }

        void set_bpm(uint32_t bpm)
        {
            _tempo_offset = (bpm - _tempo);
        }

        void toggle_pause()
        {
            _paused = !_paused;
        }

        uint16_t track_count() CONST_RETURN(_hdr.tracks);

        uint16_t ppqn() CONST_RETURN(_ppqn);
        
        bool eot() CONST_RETURN(_eot);

        HANDLER_FUNC(midi);
        HANDLER_FUNC(sysex);
        HANDLER_FUNC(meta);
    };
}

#endif // __MIDI_FILE_H__