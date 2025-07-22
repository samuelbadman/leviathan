# Set minimum CMake version required
cmake_minimum_required(VERSION "4.1.0")

# Create CMake project
project("Leviathan")

# Toggle unity build
set(UNITY_BUILD true)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../Binary")

# Add core static library module target
add_library("LeviathanCore" STATIC 
"Source/Engine/LeviathanCore/Core/PlatformEntryPointPrototype.h"
"Source/Engine/LeviathanCore/Core/Application.h"
"Source/Engine/LeviathanCore/Core/Application.cpp"
"Source/Engine/LeviathanCore/Core/Engine.h"
"Source/Engine/LeviathanCore/Core/Engine.cpp"
)

# Add target headers to precompile for core static library module
target_precompile_headers("LeviathanCore" PUBLIC 
"Source/Engine/LeviathanCore/Core/CorePrecompiledHeader.h"
)

# Begin game title executable setup
# Add game title executable target
add_executable("GameTitle" WIN32
"Source/GameTitle/TitleApplication.h" 
"Source/GameTitle/TitleApplication.cpp"
)

# Link engine static library modules used by game title executable
target_link_libraries("GameTitle" 
"LeviathanCore"
)

# Add engine module source directories as include directories for game title executable
target_include_directories("GameTitle" PRIVATE 
"Source/Engine/LeviathanCore/"
)
# End game title executable setup

# Set Visual Studio startup project. Select game title being built here
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "GameTitle")