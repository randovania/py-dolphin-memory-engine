cmake_minimum_required(VERSION 3.5.0)
project(Dolphin-memory-engine CXX)

if(WIN32)
    set(DolphinProcessSrc DolphinProcess/Windows/WindowsDolphinProcess.cpp)
elseif(UNIX)
    set(DolphinProcessSrc DolphinProcess/Linux/LinuxDolphinProcess.cpp)
else()
    set(DolphinProcessSrc DolphinProcess/Dummy/DummyDolphinProcess.cpp)
endif()

set(SRCS ${DolphinProcessSrc}
         DolphinProcess/DolphinAccessor.cpp  
         Common/MemoryCommon.cpp
         MemoryWatch/MemWatchEntry.cpp
         )

set(CMAKE_INCLUE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD 14)

add_library(dolphin-memory-engine ${SRCS})
