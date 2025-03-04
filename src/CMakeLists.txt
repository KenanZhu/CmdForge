cmake_minimum_required(VERSION 3.10)
project(MyProject)

add_subdirectory(lib)

add_executable(Demo
    Demo.cpp
    APIs.h
)

target_link_libraries(Demo 
    PRIVATE CmdForge
)

target_include_directories(Demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)