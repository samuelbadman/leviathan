# Set minimum CMake version required
cmake_minimum_required(VERSION "4.1.0")

# Create CMake project
project("Leviathan")

# Toggle unity build
set(UNITY_BUILD true)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../Binary")

# Configure build platform
set(BUILD_PLATFORM_WINDOWS ON)

# Add compile definitions to all targets in project
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
add_compile_definitions("PLATFORM_WINDOWS")
endif()

# Set executable target
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
set(BUILD_EXECUTABLE_TARGET WIN32)
endif()

# Begin static library module target setup
# Add core static library module target
add_library("LeviathanCore" STATIC 
"Source/Engine/LeviathanCore/Core/PlatformEntryPointPrototype.h"
"Source/Engine/LeviathanCore/Core/Application.h"
"Source/Engine/LeviathanCore/Core/Application.cpp"
"Source/Engine/LeviathanCore/Core/Engine.h"
"Source/Engine/LeviathanCore/Core/Engine.cpp"
)

# Add target headers to precompile for core static library module
# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanCore" PUBLIC 
"Source/Engine/LeviathanCore/Core/CorePrecompiledHeader.h"
)
# End static library module target setup

# Begin game title executable target setup
# Add game title executable target with source files compiled for all platform builds
add_executable("GameTitle" "${BUILD_EXECUTABLE_TARGET}" 
"Source/GameTitle/TitleApplication.h" 
"Source/GameTitle/TitleApplication.cpp"
)

# Add source files to be compiled only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
#target_sources("GameTitle" )
endif()

# Link engine static library modules used by game title executable
target_link_libraries("GameTitle" 
"LeviathanCore"
)

# Add engine module source directories as include directories for game title executable
target_include_directories("GameTitle" PRIVATE 
"Source/Engine/LeviathanCore/"
)
# End game title executable target setup

# Set Visual Studio startup project. Select game title being built here
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "GameTitle")