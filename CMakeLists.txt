# Set minimum CMake version required
cmake_minimum_required(VERSION "4.1.0")

# Create CMake project
project("Leviathan")

# Toggle unity build
set(UNITY_BUILD true)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../Binary")

# Add engine core static library module target
add_library("LeviathanCore" STATIC 
"Source/Engine/LeviathanCore/Core/Core.h"
"Source/Engine/LeviathanCore/Core/Core.cpp"
"Source/Engine/LeviathanCore/Core/Application.h"
"Source/Engine/LeviathanCore/Core/Application.cpp"
)

# Begin game title executable setup
# Add game title executable target
add_executable("Title" 
"Source/Title/Main.cpp" 
"Source/Title/TitleApplication.h" 
"Source/Title/TitleApplication.cpp"
)

# Link engine static library modules used by game title executable
target_link_libraries("Title" 
"LeviathanCore"
)

# Add engine module source directories as include directories for game title executable
target_include_directories("Title" PRIVATE 
"Source/Engine/LeviathanCore/"
)
# End game title executable setup

# Set Visual Studio startup project. Select game title being built here
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "Title")