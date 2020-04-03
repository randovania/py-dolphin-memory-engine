cmake_minimum_required(VERSION 3.5.0)
project(Dolphin-memory-engine)

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

add_library(py-dolphin-memory-engine SHARED ${SRCS})
