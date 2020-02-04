#ifndef __LOG_H__
#define __LOG_H__

#include "config.h"

#ifdef USE_ESP_LOG

#include <esp_log.h>

#define LOGI ESP_LOGI
#define LOGW ESP_LOGW
#define LOGE ESP_LOGE

#else

#include <stdio.h>

#define __LOG_HELPER(_f, _l, tag, ...) do \
{                                         \
    fprintf(_f, "[" _l "] (%s) ", tag);   \
    fprintf(_f, __VA_ARGS__);             \
    fprintf(_f, "\n");                    \
}                                         \
while (0)

#define LOGI(tag, ...) __LOG_HELPER(stdout, "I", tag, __VA_ARGS__)

#define LOGW(tag, ...) __LOG_HELPER(stdout, "W", tag, __VA_ARGS__)

#define LOGE(tag, ...) __LOG_HELPER(stderr, "E", tag, __VA_ARGS__)

#endif // USE_ESP_LOG

#endif // __LOG_H__