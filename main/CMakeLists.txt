idf_component_register(
    SRCS "esp_main.cpp"
         "stream_util.cpp"
         "midi_file.cpp"
         "midi_track.cpp"
         "midi_util.cpp"
         "gpio/gpio_esp32.c"
         "floppy/floppy_playback.cpp"
    INCLUDE_DIRS "."
)

spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
