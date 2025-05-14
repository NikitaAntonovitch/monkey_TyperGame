cmake_minimum_required(VERSION 3.24)
project(monkey_TyperGame)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Don't build SFML shared libraries
set(BUILD_SHARED_LIBS OFF)

# Add SFML via FetchContent
include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(monkey_TyperGame
        main.cpp
        Utils.cpp
        Utils.h
        DropdownCtrl.cpp
        DropdownCtrl.h
        Configuration.cpp
        Configuration.h
)
# Link SFML modules
target_link_libraries(monkey_TyperGame
        sfml-graphics
        sfml-window
        sfml-system
)

# Include current dir for headers
target_include_directories(monkey_TyperGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
