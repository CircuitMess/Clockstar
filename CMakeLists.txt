cmake_minimum_required(VERSION 3.15)
project(CircuitWatch)

set(CMAKE_CXX_STANDARD 11)

include_directories(~/.arduino15/packages/cm/hardware/esp32/1.0.4/cores/esp32)
include_directories(~/.arduino15/packages/cm/hardware/esp32/1.0.4/tools/sdk/include)
include_directories(~/Arduino/libraries/RTClib)
include_directories(~/Arduino/libraries/TFT_eSPI-master)
include_directories(~/Arduino/libraries/CircuitOS/src)

AUX_SOURCE_DIRECTORY(src SOURCES)
AUX_SOURCE_DIRECTORY(src/Screens SOURCES)
AUX_SOURCE_DIRECTORY(src/Bitmaps SOURCES)

add_definitions(-DDEBUG)

SET_SOURCE_FILES_PROPERTIES(CircuitWatch.ino PROPERTIES LANGUAGE CXX)
add_executable(SmartWatch ${SOURCES} CircuitWatch.ino)