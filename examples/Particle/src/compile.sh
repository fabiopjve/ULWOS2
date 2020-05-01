#!/bin/bash
# Particle webcompiler expects C++ files, since we can't send compiler switches,
# we need to rename ULWOS source files
mv ../../../src/ULWOS2.c ../../../src/ULWOS2.cpp
mv ../../../src/ULWOS2_HAL.c ../../../src/ULWOS2_HAL.cpp
particle compile photon ledBlinker.cpp ../../../src/ULWOS2.cpp ../../../src/ULWOS2.h ../../../src/ULWOS2_HAL.cpp ../../../src/ULWOS2_HAL.h ../../../src/ULWOS2_config.h --saveTo ledBlinker.bin
# Now restore ULWOS source file names
mv ../../../src/ULWOS2.cpp ../../../src/ULWOS2.c
mv ../../../src/ULWOS2_HAL.cpp ../../../src/ULWOS2_HAL.c