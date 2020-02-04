#ifndef __MIDI_UTIL_H__
#define __MIDI_UTIL_H__

#include "midi_const.h"

#include <cstddef>
#include <cstdint>

namespace midi
{
    const char* get_note_name(int8_t note);

    inline int8_t calc_note_octave(int8_t note)
    {
        return ((note - (note % 12)) / 12) - 1;
    }

    inline uint64_t calc_tick_micros(uint32_t tick, uint16_t ppqn, uint32_t tempo)
    {
        return ((tick * MSPM) / (float)ppqn) / (float)tempo;
    }

    inline uint32_t calc_bpm(uint32_t mpqn)
    {
        return MSPM / mpqn;
    }

    inline uint32_t calc_mpqn(uint32_t bpm)
    {
        return calc_bpm(bpm);
    }
}

#endif // __MIDI_UTIL_H__