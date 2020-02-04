#include "floppy/floppy_playback.h"

#include "midi_ev_id.h"

namespace midi
{
    namespace floppy
    {
        static const uint16_t* _note_periods_div = _c.arr;

        void floppy_playback::_pulse_drive(uint8_t c)
        {
            floppy_pin_pair_t pins = _channel_pins[c];

            uint8_t pos = (_drive_pos[c] += _drive_dirs[c] ? -1 : 1);

            if ((pos >= MAX_FLOPPY_POS) || (pos <= MIN_FLOPPY_POS))
                gpiocompat_set_pin(pins.direction, _drive_dirs[c] = !_drive_dirs[c]);

            gpiocompat_set_pin(pins.step, _drive_states[c] = !_drive_states[c]);
        }

        void floppy_playback::_on_midi_event(midi_event_t ev)
        {
            if (ev.type == MIDI_NOTE_ON && ev.params[1] == 0)
                ev.type = MIDI_NOTE_OFF;

            switch (ev.type)
            {
                case MIDI_NOTE_ON:
                    _set_channel_period(ev.channel, _note_periods_div[ev.params[0]]);
                    break;

                case MIDI_NOTE_OFF:
                    _set_channel_period(ev.channel, 0);
                    break;

                case MIDI_PITCH_BEND:
                    int16_t bend = ((ev.params[1] << 7) | ev.params[0]);

                    _bend_channel(ev.channel, bend - CENTRAL_BEND);
                    break;
            }
        }

        void floppy_playback::_on_sysex_event(sysex_event_t ev)
        { }

        void floppy_playback::_on_meta_event(meta_event_t ev)
        { }

        void floppy_playback::set_channel_pins(uint8_t channel, gpiocompat_pin_t step, gpiocompat_pin_t direction)
        {
            if (!channel) return;
            if (step) gpiocompat_init_pin(step, GPIOCOMPAT_MODE_OUTPUT);
            if (direction) gpiocompat_init_pin(direction, GPIOCOMPAT_MODE_OUTPUT);

            _channel_pins[channel - 1] = {step, direction};
        }

        void floppy_playback::reset_drives()
        {
            for (uint8_t c = 0; c < MAX_CHANNELS; c++)
            {
                floppy_pin_pair_t pins = _channel_pins[c];

                gpiocompat_set_pin(pins.direction, !_drive_dirs[c]);
                gpiocompat_set_pin(pins.step, false);

                for (uint16_t i = 0; i < MAX_FLOPPY_POS * 2; i++)
                {
                    gpiocompat_set_pin(pins.step, true);
                    gpiocompat_set_pin(pins.step, false);
                }
            }
        }

        void floppy_playback::tick()
        {
            for (uint8_t i = 0; i < MAX_CHANNELS; i++)
            {
                if (!_channel_periods_bent[i]) continue;

                if ((++_channel_ticks[i]) >= (_channel_periods_bent[i]))
                {
                    _pulse_drive(i);

                    _channel_ticks[i] = 0;
                }
            }
        }
    }
}