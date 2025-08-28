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
# Configuration definitions
add_compile_definitions("$<$<CONFIG:Debug>:CONFIG_DEBUG>")
add_compile_definitions("$<$<CONFIG:Release>:CONFIG_RELEASE>")
add_compile_definitions("$<$<CONFIG:RelWithDebInfo>:CONFIG_RELWITHDEBINFO;CONFIG_RELEASE>")
add_compile_definitions("$<$<CONFIG:MinSizeRel>:CONFIG_MINSIZEREL;CONFIG_RELEASE>")

# Platform definitions
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
add_compile_definitions("PLATFORM_WINDOWS")
endif()

# Set executable target
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
set(BUILD_EXECUTABLE_TARGET WIN32)
endif()

#################################################################################################
# Modules
#################################################################################################

#------------------------------------------------------------------------------------------------
# Core module
#------------------------------------------------------------------------------------------------

# Every title must include core as a target module
# Begin core module static library module target setup
# Add core static library module target with source files to be compiled for all platforms
add_library("LeviathanCore" STATIC 
"Source/Engine/LeviathanCore/Core/Platform/PlatformEntryPointPrototype.h"
"Source/Engine/LeviathanCore/Core/Platform/PlatformEntryPointImplementation.h"
"Source/Engine/LeviathanCore/Core/Platform/Platform.h"
"Source/Engine/LeviathanCore/Core/Platform/Gamepad.h"
"Source/Engine/LeviathanCore/Core/ProgrammingTools/Delegate.h"
"Source/Engine/LeviathanCore/Core/ProgrammingTools/Bits.h"
"Source/Engine/LeviathanCore/Core/Application.h"
"Source/Engine/LeviathanCore/Core/Application.cpp"
"Source/Engine/LeviathanCore/Core/Engine.h"
"Source/Engine/LeviathanCore/Core/Engine.cpp"
"Source/Engine/LeviathanCore/Core/ConsoleOutput.h"
"Source/Engine/LeviathanCore/Core/NotificationManager.h"
"Source/Engine/LeviathanCore/Core/NotificationManager.cpp"
"Source/Engine/LeviathanCore/Core/Window.h"
"Source/Engine/LeviathanCore/Core/Window.cpp"
"Source/Engine/LeviathanCore/Core/InputKey.h"
"Source/Engine/LeviathanCore/Core/InputKey.cpp"
"Source/Engine/LeviathanCore/Core/Module.h"
"Source/Engine/LeviathanCore/Core/Module.cpp"
)

# Add source files to be compiled and libraries to link against only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
target_sources("LeviathanCore" PRIVATE
"Source/Engine/LeviathanCore/Core/Platform/Windows/Platform_Windows.cpp"
"Source/Engine/LeviathanCore/Core/Platform/Windows/Gamepad_XInput.cpp"
"Source/Engine/LeviathanCore/Core/Platform/Windows/Definitions_Windows.h"
)

target_link_libraries("LeviathanCore" 
"xinput.lib"
)
endif()

# Add target headers to precompile for core static library module
# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanCore" PUBLIC 
"Source/Engine/LeviathanCore/Core/CorePrecompiledHeader.h"
)

# Add static library module include directories
target_include_directories("LeviathanCore" PRIVATE 
"Source/Engine/LeviathanCore/"
)
# End core module static library module target setup

#------------------------------------------------------------------------------------------------
# Renderer module
#------------------------------------------------------------------------------------------------

# Begin renderer module static library module target setup
# Add renderer static library module target with source files to be compiled for all platforms
add_library("LeviathanRenderer" STATIC 
"Source/Engine/LeviathanRenderer/Renderer/RendererModule.h"
"Source/Engine/LeviathanRenderer/Renderer/RendererModule.cpp"
# TODO: Add switch to specify which rendering API to compile the engine with
"Source/Engine/LeviathanRenderer/Vendor/glad/glad.cpp"
)

# Add source files to be compiled and libraries to link against only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
target_sources("LeviathanRenderer" PRIVATE
""
)

target_link_libraries("LeviathanRenderer" 
""
)
endif()

# Add target headers to precompile for renderer static library module
# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanRenderer" PUBLIC 
# Modules need to use the core module precompiled header
"Source/Engine/LeviathanCore/Core/CorePrecompiledHeader.h" 
"Source/Engine/LeviathanRenderer/Renderer/RendererPrecompiledHeader.h"
)

# Add static library module include directories
target_include_directories("LeviathanRenderer" PRIVATE 
"Source/Engine/LeviathanCore/"
"Source/Engine/LeviathanRenderer/"
# TODO: Add switch to specify which rendering API to compile the engine with
"Source/Engine/LeviathanRenderer/Vendor/glad/"
)
# End renderer module static library module target setup

#################################################################################################
# Titles
#################################################################################################

#------------------------------------------------------------------------------------------------
# Game title
#------------------------------------------------------------------------------------------------

# Begin game title executable target setup
# Add game title executable target with source files compiled for all platform builds
add_executable("GameTitle" "${BUILD_EXECUTABLE_TARGET}" 
"Source/GameTitle/TitleApplication.h" 
"Source/GameTitle/TitleApplication.cpp"
"Source/GameTitle/TitleApplicationWindow.h"
"Source/GameTitle/TitleApplicationWindow.cpp"
)

# Link engine static library modules used by game title executable
target_link_libraries("GameTitle" 
"LeviathanCore"
"LeviathanRenderer"
)

# Add engine static library module source directories as include directories for game title executable
target_include_directories("GameTitle" PRIVATE 
"Source/Engine/LeviathanCore/"
"Source/Engine/LeviathanRenderer/"
)
# End game title executable target setup

#################################################################################################
#################################################################################################

# Set Visual Studio startup project. Select game title being built here
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "GameTitle")