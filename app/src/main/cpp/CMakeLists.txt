cmake_minimum_required(VERSION 3.18.1)

project("touchgame")

# Add the game source files
add_library(touchgame SHARED
    native-lib.cpp
    game.cpp
)

# Find required libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv2-lib GLESv2)

# Link libraries
target_link_libraries(touchgame
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv2-lib}
)
