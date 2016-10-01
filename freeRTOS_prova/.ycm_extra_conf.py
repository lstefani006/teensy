"""
YouCompleteMe extra configuration for Platformio based
projects.

Based on the `.ycm_extra_conf.py` by @ladislas in his Bare-Arduino-Project.

Anthony Ford <github.com/ajford>

"""
import os
import ycm_core
import logging

flags = [
        "-nostdinc"
        ,"-nostdinc++"
        ,"-DDALLAS"
        ,"-D__ATTR_PROGMEM__=/**/"
        ,"-D__AVR_ARCH__=5"
        ,"-D__AVR_ATmega328P__"
        ,"-std=gnu++14"
        ,'-Wall'
        ,'-x' ,'c++'
        ,'-mmcu=atmega328p'
        ,'-DF_CPU=8000000L'
        ,'-DARDUINO_ARCH_AVR'
        ,'-DARDUINO_AVR_PRO'
        ,'-DARDUINO=106008'
        ,"-I/home/leo/arduino-1.6.11/hardware/arduino/avr/cores/arduino"
        ,"-I/home/leo/arduino-1.6.11/hardware/tools/avr/avr/include/"
        ,"-I/home/leo/Arduino/libraries/FreeRTOS/src"
        ,"-I/home/leo/arduino-1.6.11/hardware/arduino/avr/variants/eightanaloginputs"
        ,"-DNULL=nullptr"
        ]


def FlagsForFile(filename, **kwargs):
    return { 'flags': flags, 'do_cache': True }
