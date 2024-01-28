cmake_minimum_required(VERSION 3.16)
project(pnx_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${EIGEN3_INCLUDE_DIRS}
)

include_directories(
        AngleSolver/include
        ArmorDetector/include
        Comm/Protocol
        Comm/Protocol/CRC
        Comm/Serial/
        Data/include
        ThreadManager/include
)

file(GLOB_RECURSE source
        AngleSolver/src/*
        ArmorDetector/src/*
        ArmorDetector/include/*
        Comm/*
        Serial/*
        ThreadManager/src/*
        )

add_executable(pnx_demo main.cpp ${source})

target_link_libraries(pnx_demo
        ${OpenCV_LIBS}
        ${Eigen3_LIBRARIES}
        -lpthread -lm
        )