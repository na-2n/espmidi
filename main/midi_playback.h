#ifndef __MIDI_PLAYBACK_H__
#define __MIDI_PLAYBACK_H__

#include "midi_event.h"

#include <cstddef>
#include <cstdint>

namespace midi
{
    class midi_playback
    {
        friend class midi_file;

    protected:
        virtual void _on_midi_event(midi_event_t ev);

        virtual void _on_meta_event(meta_event_t ev);

        virtual void _on_sysex_event(sysex_event_t ev);

    public:
        virtual ~midi_playback()
        { };
    };
}

#endif // __MIDI_PLAYBACK_H__