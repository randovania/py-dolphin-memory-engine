cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GCC_min_version 10)
project(dolphin-memory-engine)

if(WIN32)
    set(DolphinProcessSrc DolphinProcess/Windows/WindowsDolphinProcess.cpp)
endif(WIN32)

if(UNIX AND NOT APPLE)
   set(DolphinProcessSrc DolphinProcess/Linux/LinuxDolphinProcess.cpp)
endif(UNIX AND NOT APPLE)

if(APPLE)
   set(DolphinProcessSrc DolphinProcess/Mac/MacDolphinProcess.cpp)
endif(APPLE)

set(SRCS ${DolphinProcessSrc}
         DolphinProcess/DolphinAccessor.cpp
         Common/MemoryCommon.cpp
         MemoryWatch/MemWatchEntry.cpp
         MemoryScanner/MemoryScanner.cpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(dolphin-memory-engine ${SRCS})
