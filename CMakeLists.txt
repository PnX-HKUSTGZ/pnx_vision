cmake_minimum_required(VERSION 3.16)
project(pnx_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/IndustrialCamera/lib/linux/*)

include_directories(
        AngleSolver/include
        ArmorDetector/include
        Comm/Protocol
        Comm/Protocol/CRC
        Comm/Serial/
        Data/include
        ThreadManager/include
        IndustrialCamera/include
)

file(GLOB_RECURSE source
        AngleSolver/src/*
        ArmorDetector/src/*
        ArmorDetector/include/*
        Comm/*
        Serial/*
        ThreadManager/src/*
        IndustrialCamera/src/*
        )

add_executable(pnx_demo main.cpp ${source})


target_link_libraries(pnx_demo
        ${OpenCV_LIBS}
        ${SDK_PATH}
        -lpthread -lm
        )