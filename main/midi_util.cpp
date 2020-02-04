#include "midi_util.h"

namespace midi
{
    const char* get_note_name(int8_t note)
    {
        return NOTE_NAMES[note % 12];   
    }
}