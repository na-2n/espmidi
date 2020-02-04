#ifndef __MIDI_CONST_H__
#define __MIDI_CONST_H__

#include <cstddef>
#include <cstdint>

namespace midi
{
    static const uint32_t DEFAULT_TEMPO = 120;
    static const uint16_t DEFAULT_PPQN = 48;
    
    static const int16_t CENTRAL_BEND = 0x2000;
    static const uint8_t MAX_CHANNELS = 16;
    static const int16_t MAX_BEND = 0x3FFF;
    static constexpr int16_t MAX_CENTRALIZED_BEND = MAX_BEND - CENTRAL_BEND;
    static const int16_t MIN_CENTRALIZED_BEND = ~MAX_CENTRALIZED_BEND;

    static const uint32_t MSPM = 60000000;

    const char NOTE_NAMES[12][3] =
    {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    static const char HEADER_CHUNK_ID[5] = "MThd";
    static const char TRACK_CHUNK_ID[5] = "MTrk";
}

#endif // __MIDI_CONST_H__