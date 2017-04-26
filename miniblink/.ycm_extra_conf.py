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
        "-std=gnu++14"
        ,'-Wall'
        ,'-x' ,'c++'
        ,'-I/home/leo/teensy/LibStm/libopencm3/include'
        ,'-DSTM32F1'
        ]



def FlagsForFile(filename, **kwargs):
    return { 'flags': flags, 'do_cache': True }
