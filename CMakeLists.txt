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
"Source/Engine/LeviathanCore/Core/Platform/Uuid.h"
)

# Add source files to be compiled and libraries to link against only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
target_sources("LeviathanCore" PRIVATE
"Source/Engine/LeviathanCore/Core/Platform/Windows/Platform_Windows.cpp"
"Source/Engine/LeviathanCore/Core/Platform/Windows/Gamepad_XInput.cpp"
"Source/Engine/LeviathanCore/Core/Platform/Windows/Definitions_Windows.h"
"Source/Engine/LeviathanCore/Core/Platform/Windows/Uuid_Windows.cpp"
)

target_link_libraries("LeviathanCore" 
"xinput.lib"
"Rpcrt4.lib" # Uuids
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
# Rendering module
#------------------------------------------------------------------------------------------------

# Begin rendering module static library module target setup
# Add rendering static library module target with source files to be compiled for all platforms
add_library("LeviathanRendering" STATIC 
"Source/Engine/LeviathanRendering/Rendering/RenderingModule.h"
"Source/Engine/LeviathanRendering/Rendering/RenderingModule.cpp"
"Source/Engine/LeviathanRendering/Rendering/RenderHardwareInterface/RenderHardwareInterface.h"
)

# Add source files for rendering api being compiled
# TODO: Add switch to specify which rendering API to compile the engine with
target_sources("LeviathanRendering" PRIVATE 
"Source/Engine/LeviathanRendering/Vendor/glad/glad.cpp"
"Source/Engine/LeviathanRendering/Rendering/RenderHardwareInterface/OpenGL/RenderHardwareInterface_OpenGL.cpp"
)

# Add source files to be compiled and libraries to link against only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
#target_sources("LeviathanRendering" PRIVATE
#""
#)

target_link_libraries("LeviathanRendering" 
# TODO: Add switch for which rendering api libraries to link against
"Opengl32.lib"
)
endif()

# Add target headers to precompile for rendering static library module
# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanRendering" PUBLIC 
"Source/Engine/LeviathanRendering/Rendering/RenderingPrecompiledHeader.h"
"Source/Engine/LeviathanCore/Core/CorePrecompiledHeader.h"
)

# Add static library module include directories
target_include_directories("LeviathanRendering" PRIVATE 
"Source/Engine/LeviathanCore/"
"Source/Engine/LeviathanRendering/"
# TODO: Add switch to specify which rendering API to compile the engine with
"Source/Engine/LeviathanRendering/Vendor/glad/"
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
"LeviathanRendering"
)

# Add engine static library module source directories as include directories for game title executable
target_include_directories("GameTitle" PRIVATE 
"Source/Engine/LeviathanCore/"
"Source/Engine/LeviathanRendering/"
)
# End game title executable target setup

#################################################################################################
#################################################################################################

# Set Visual Studio startup project. Select game title being built here
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "GameTitle")

# Set working directory for game title being built to be the same as the game title target output directory
set_property(TARGET "GameTitle" PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:GameTitle>")

# Build commands
# Copy rendering module shader source files to build target directory
# TODO: Only do this if the rendering module is being built
# TODO: Add switch for rendering api being used to only copy the shader source files for the api being used
add_custom_command(TARGET "GameTitle" POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Source/Engine/LeviathanRendering/Shaders/OpenGL" 
"$<TARGET_FILE_DIR:GameTitle>/Shaders")