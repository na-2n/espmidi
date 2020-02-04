#ifndef __MIDI_EV_ID_H__
#define __MIDI_EV_ID_H__

namespace midi
{
    typedef enum
    {
        MIDI_NOTE_OFF = 0x08,
        MIDI_NOTE_ON,
        MIDI_POLYPHONIC_PRESSURE,
        MIDI_CONTROLLER,
        MIDI_PROGRAM_CHANGE,
        MIDI_CHANNEL_PRESSURE,
        MIDI_PITCH_BEND,
    } midi_ev_id_t;

    typedef enum
    {
        META_SEQUENCE_NUMBER = 0x00,
        META_TEXT,
        META_COPYRIGHT,
        META_SEQUENCE_NAME,
        META_INSTRUMENT_NAME,
        META_LYRIC,
        META_MARKER,
        META_CUE_POINT,
        META_PROGRAM_NAME,

        META_CHANNEL_PREFIX = 0x20,
        META_PORT,

        META_EOT = 0x2F, // EOT = End Of Track

        META_TEMPO = 0x51,

        META_SMPTE_OFFSET = 0x54,

        META_TIME_SIGNATURE = 0x58,
        META_KEY_SIGNATURE,

        META_SEQUENCER_SPECIFIC = 0x7F,
    } meta_ev_id_t;
}

#endif //__MIDI_EV_ID_H__