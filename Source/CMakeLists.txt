cmake_minimum_required(VERSION 3.5.0)
project(Dolphin-memory-engine CXX)

if(WIN32)
    set(DolphinProcessSrc DolphinProcess/Windows/WindowsDolphinProcess.cpp)
endif(WIN32)

if(UNIX)
   set(DolphinProcessSrc DolphinProcess/Linux/LinuxDolphinProcess.cpp)
endif(UNIX)

set(SRCS ${DolphinProcessSrc}
         DolphinProcess/DolphinAccessor.cpp  
         Common/MemoryCommon.cpp
         MemoryWatch/MemWatchEntry.cpp   
         
         MemoryScanner/MemoryScanner.cpp
         )

set(CMAKE_INCLUE_CURRENT_DIR ON)
set(CXX_STANDARD_REQUIRED YES)
set(CXX_STANDARD 14)

add_library(dolphin-memory-engine ${SRCS})
