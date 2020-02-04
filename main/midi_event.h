#ifndef __MIDI_EVENT_H__
#define __MIDI_EVENT_H__

#include "preproc.h"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <functional>

#define HANDLER_TYPE(type) \
    typedef std::function<void (type##_event_t)> type##_ev_handler_t

#define HANDLER_FIELD(type) \
    std::vector<type##_ev_handler_t> _##type##_handlers 

#define HANDLER_FUNC(type)                     \
    void on_##type##_ev(type##_ev_handler_t f) \
    {                                          \
        _##type##_handlers.push_back(f);       \
    }

#define INTERNAL_EV_FUNC(type) \
    void _internal_##type##_ev(type##_event_t ev);

#define TRIGGER_EV(type, ev)                         \
    for (type##_ev_handler_t f : _##type##_handlers) \
        f(ev)

#define TRIGGER_FUNC(type)                       \
    void _trigger_##type##_ev(type##_event_t ev) \
    {                                            \
        TRIGGER_EV(type, ev);                    \
    }

#define _PROXY_INT(type) \
    on_##type##_ev([this](type##_event_t ev) { this->_trigger_##type##_ev(ev); })

#define _PROXY_EXT(type, func) \
    on_##type##_ev([this](type##_event_t ev) { func(ev); })

#define proxy_event(...) _GET_ARG3(__VA_ARGS__, _PROXY_EXT, _PROXY_INT)(__VA_ARGS__)

#define internal_event(type) \
    on_##type##_ev([this](type##_event_t ev) { this->_internal_##type##_ev(ev); })

namespace midi
{
    typedef struct
    {
        uint8_t type : 4;
        uint8_t channel : 4;
        int8_t params[2];
    } midi_event_t;

    typedef struct
    {
        uint8_t type;
        size_t length;
        char* data;
    } meta_event_t;

    typedef struct
    {
        size_t length;
        char* data;
    } sysex_event_t;

    HANDLER_TYPE(midi);
    HANDLER_TYPE(sysex);
    HANDLER_TYPE(meta);
}

#endif // __MIDI_EVENT_H__