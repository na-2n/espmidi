#ifndef __FLOPPY_PLAYBACK_H__
#define __FLOPPY_PLAYBACK_H__

#include "config.h"

#include "midi_const.h"
#include "midi_event.h"
#include "midi_playback.h"

#include "gpio/gpio_compat.h"

#include <cstdint>
#include <cmath>

#ifndef PLAYBACK_INTERVAL
#define PLAYBACK_INTERVAL 1 // one microsecond
#endif // PLAYBACK_INTERVAL

namespace midi
{
    namespace floppy
    {
        // https://github.com/SammyIAm/Moppy2/blob/master/Arduino/Moppy/src/MoppyInstruments/MoppyInstrument.h

        static constexpr float BEND_OCTAVES = 200.0f / 1200.0f;

        static const uint8_t MAX_FLOPPY_POS = 158;
        static const uint8_t MIN_FLOPPY_POS = 0;

        #define PERIOD_MAX 128

        constexpr uint16_t _note_periods[PERIOD_MAX] =
        {
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            30578, 28861, 27242, 25713, 24270, 22909, 21622, 20409, 19263, 18182, 17161, 16198, // C1 - B1
            15289, 14436, 13621, 12856, 12135, 11454, 10811, 10205,  9632,  9091,  8581,  8099, // C2 - B2
             7645,  7218,  6811,  6428,  6068,  5727,  5406,  5103,  4816,  4546,  4291,  4050, // C3 - B3
             3823,  3609,  3406,  3214,  3034,  2864,  2703,  2552,  2408,  2273,  2146,  2025, // C4 - B4
             1911,  1804,  1703,  1607,  1517,  1432,  1351,  1276,  1204,  1136,  1073,  1012, // C5 - B5
              956,   902,   851,   804,   758,   716,   676,   638,   602,   568,   536,   506, // C6 - B6
              478,   451,   426,   402,   379,   358,   338,   319,   301,   284,   268,   253, // C7 - B7
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0
        };

        template<uint64_t micros>
        struct _calc_periods
        {
            constexpr _calc_periods() : arr()
            { 
                for (uint8_t i = 0; i < PERIOD_MAX; i++)
                {
                    if ((arr[i] = _note_periods[i]))
                        arr[i] /= micros;
                }
            }

            uint16_t arr[PERIOD_MAX];
        };

        constexpr _calc_periods<PLAYBACK_INTERVAL * 2> _c;

        #undef PERIOD_MAX

        inline uint16_t calc_bend(int16_t bend, uint16_t period)
        {
            return period / pow(2, (BEND_OCTAVES * (bend / (float)CENTRAL_BEND)));
        }

        typedef struct
        {
            gpiocompat_pin_t step;
            gpiocompat_pin_t direction;
        } floppy_pin_pair_t;

        class floppy_playback : public midi_playback
        {
        private:
            uint16_t _channel_periods[MAX_CHANNELS] = {0};
            uint16_t _channel_periods_bent[MAX_CHANNELS] = {0};
            uint16_t _channel_ticks[MAX_CHANNELS] = {0};

            gpiocompat_dval_t _drive_states[MAX_CHANNELS] = {LOW};
            bool _drive_dirs[MAX_CHANNELS] = {0};
            uint8_t _drive_pos[MAX_CHANNELS] = {0};

            floppy_pin_pair_t _channel_pins[MAX_CHANNELS];

        protected:
            void _on_midi_event(midi_event_t ev);
            void _on_sysex_event(sysex_event_t ev);
            void _on_meta_event(meta_event_t ev);

            void _pulse_drive(uint8_t channel);

            inline void _set_channel_period(uint8_t channel, uint16_t period = 0)
            {
                _channel_periods_bent[channel] = _channel_periods[channel] = period;
            }

            inline void _bend_channel(uint8_t channel, int16_t bend)
            {
                if (bend > MAX_CENTRALIZED_BEND) bend = MAX_CENTRALIZED_BEND;
                if (bend < MIN_CENTRALIZED_BEND) bend = MIN_CENTRALIZED_BEND;

                _channel_periods_bent[channel] = calc_bend(bend, _channel_periods[channel]);
            }

        public:
            floppy_playback()
            { }

            ~floppy_playback()
            { }

            void reset_drives();

            void set_channel_pins(uint8_t channel, gpiocompat_pin_t step, gpiocompat_pin_t direction);

            void tick();
        };
    }
}

#endif // __FLOPPY_PLAYBACK_H__