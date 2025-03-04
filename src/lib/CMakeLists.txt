add_library(CmdForge STATIC
    CmdForge.cpp
    CmdForge.h
)

target_include_directories(CmdForge PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)