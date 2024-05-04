#!/bin/bash
~/.platformio/penv/bin/python ~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 merge_bin -o merged_firmware_0x0.bin --flash_mode dio \
 0x0000 .pio/build/esp32-s3-devkitc-1/bootloader.bin \
 0x8000 .pio/build/esp32-s3-devkitc-1/partitions.bin \
 0xe000 ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin \
 0x10000 .pio/build/esp32-s3-devkitc-1/firmware.bin
