// Device : ESP32 DEVKIT V1

#include "config.h"
#include "log.h"

#include "midi_file.h"
#include "midi_const.h"

#include "floppy/floppy_playback.h"

#include <cstdint>
#include <fstream>
#include <thread>
#include <chrono>

#include <esp_types.h>
#include <esp_pthread.h>
#include <esp_spiffs.h>
#include <esp_err.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "ESP-MIDI";

static const char* GPL_TEXT = R"GPL(
espmidi  Copyright (C) 2020  Yui
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to
redistribute it under certain conditions.
)GPL";

static const midi::floppy::floppy_pin_pair_t _pins[midi::MAX_CHANNELS] = PIN_LAYOUT;
static const uint8_t _reroute[midi::MAX_CHANNELS] = CHANNEL_REROUTE;

midi::floppy::floppy_playback playback{};

void floppy_task(void* p)
{
    for (;;)
    {
        playback.tick();

        std::this_thread::sleep_for(std::chrono::microseconds{PLAYBACK_INTERVAL});
    }
}

extern "C" void app_main()
{
    LOGI(TAG, "%s", GPL_TEXT);

    LOGI(TAG, "Initializing SPIFFS");
    
    esp_vfs_spiffs_conf_t vfs_cfg = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    
    esp_err_t ret = esp_vfs_spiffs_register(&vfs_cfg);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    std::ifstream f{"/spiffs/e1m1.mid", std::fstream::in | std::fstream::binary};

    if (!f.good())
    {
        LOGE(TAG, "Failed to find 'audio.mid' on SD");

        return;   
    }

    midi::midi_file mid{&f};

    mid.use_playback(&playback);

    for (uint8_t i = 0; i < midi::MAX_CHANNELS; i++)
    {
        midi::floppy::floppy_pin_pair_t pair = _pins[i];

        uint8_t c = _reroute[i];

        if (!c) c = i;

        LOGI(TAG, "Channel %u: step=%u, dir=%u", c+1, pair.step, pair.direction);

        playback.set_channel_pins(c+1, pair.step, pair.direction);
    }

    playback.reset_drives();

    LOGI(TAG, "Loading MIDI file...");

    mid.load();

    LOGI(TAG, "Playing file...");

    TaskHandle_t handle = NULL;

    xTaskCreatePinnedToCore(&floppy_task, "Floppy Task", 8096, NULL, 10, &handle, 1);

    for (; mid.next_event() != midi::EOT;) ;

    if (handle != NULL) vTaskDelete(handle);

    LOGI(TAG, "Track ended");

    esp_vfs_spiffs_unregister(NULL);
}

