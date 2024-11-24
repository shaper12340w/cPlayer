cmake_minimum_required(VERSION 3.29)
project(cPlayer_practice)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdump-rtl-expand")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdump-rtl-expand")

include_directories(.)

add_executable(cPlayer_practice
    dynamicArray.cpp
    dynamicArray.h
    fileManager.cpp
    fileMannger.h
    main.cpp
    musicPlayer.cpp
    musicPlayer.h
    playerUI.cpp
    playerUI.h)

target_link_libraries(cPlayer_practice winmm shlwapi)