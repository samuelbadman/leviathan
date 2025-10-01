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
"Source/Engine/LeviathanCore/Public/Engine.h"
"Source/Engine/LeviathanCore/Public/FileIOManager.h"
"Source/Engine/LeviathanCore/Public/Window.h"
"Source/Engine/LeviathanCore/Public/Application.h"
"Source/Engine/LeviathanCore/Public/ConsoleOutput.h"
"Source/Engine/LeviathanCore/Public/NotificationManager.h"
"Source/Engine/LeviathanCore/Public/Platform/PlatformEntryPointImplementation.h"
"Source/Engine/LeviathanCore/Public/Platform/PlatformEntryPointPrototype.h"
"Source/Engine/LeviathanCore/Public/Module.h"
"Source/Engine/LeviathanCore/Public/InputKey.h"
"Source/Engine/LeviathanCore/Public/ProgrammingTools/Delegate.h"
"Source/Engine/LeviathanCore/Public/ProgrammingTools/Bits.h"

"Source/Engine/LeviathanCore/Private/Platform/Platform.h"
"Source/Engine/LeviathanCore/Private/Platform/Gamepad.h"
"Source/Engine/LeviathanCore/Private/Application.cpp"
"Source/Engine/LeviathanCore/Private/Engine.cpp"
"Source/Engine/LeviathanCore/Private/NotificationManager.cpp"
"Source/Engine/LeviathanCore/Private/Window.cpp"
"Source/Engine/LeviathanCore/Private/InputKey.cpp"
"Source/Engine/LeviathanCore/Private/Module.cpp"
"Source/Engine/LeviathanCore/Private/Platform/Uuid.h"
"Source/Engine/LeviathanCore/Private/FileIOManager.cpp"
)

# Add source files to be compiled and libraries to link against only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
target_sources("LeviathanCore" PRIVATE
"Source/Engine/LeviathanCore/Private/Platform/Windows/Platform_Windows.cpp"
"Source/Engine/LeviathanCore/Private/Platform/Windows/Gamepad_XInput.cpp"
"Source/Engine/LeviathanCore/Private/Platform/Windows/Definitions_Windows.h"
"Source/Engine/LeviathanCore/Private/Platform/Windows/Uuid_Windows.cpp"
)

target_link_libraries("LeviathanCore" 
"xinput.lib"
"Rpcrt4.lib" # Used for uuid implementation
)
endif()

# Add target headers to precompile for core static library module
# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanCore" PUBLIC 
"Source/Engine/LeviathanCore/Public/CorePrecompiledHeader.h"
)

# Add static library module include directories
target_include_directories("LeviathanCore" PRIVATE 
"Source/Engine/LeviathanCore/Public/"
"Source/Engine/LeviathanCore/Private/"
)
# End core module static library module target setup

#------------------------------------------------------------------------------------------------
# Rendering abstraction module
#------------------------------------------------------------------------------------------------

# Begin rendering abstraction module static library module target setup
# Add rendering abstraction static library module target with source files to be compiled for all platforms
add_library("LeviathanRenderingAbstraction" STATIC 
"Source/Engine/LeviathanRenderingAbstraction/RenderingAbstraction/RenderingAbstractionModule.h"
"Source/Engine/LeviathanRenderingAbstraction/RenderingAbstraction/RenderingAbstractionModule.cpp"
"Source/Engine/LeviathanRenderingAbstraction/RenderingAbstraction/RenderHardwareInterface/RenderHardwareInterface.h"
)

# Add source files for rendering api being compiled
# TODO: Add switch to specify which rendering API to compile the engine with
target_sources("LeviathanRenderingAbstraction" PRIVATE 
"Source/Engine/LeviathanRenderingAbstraction/Vendor/glad/glad.cpp"
"Source/Engine/LeviathanRenderingAbstraction/RenderingAbstraction/RenderHardwareInterface/OpenGL/RenderHardwareInterface_OpenGL.cpp"
)

# Add source files to be compiled and libraries to link against only for specific platform builds
if("${BUILD_PLATFORM_WINDOWS}" MATCHES ON)
#target_sources("LeviathanRenderingAbstraction" PRIVATE
#""
#)

target_link_libraries("LeviathanRenderingAbstraction" 
# TODO: Add switch for which rendering api libraries to link against
"Opengl32.lib"
)
endif()

# Add target headers to precompile for rendering abstraction static library module
# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanRenderingAbstraction" PUBLIC 
"Source/Engine/LeviathanRenderingAbstraction/RenderingAbstraction/RenderingAbstractionPrecompiledHeader.h"
"Source/Engine/LeviathanCore/Public/CorePrecompiledHeader.h"
)

# Add static library module include directories
target_include_directories("LeviathanRenderingAbstraction" PRIVATE 
"Source/Engine/LeviathanCore/Public/"
"Source/Engine/LeviathanRenderingAbstraction/"
# TODO: Add switch to specify which rendering API vendor sources to compile the engine with
"Source/Engine/LeviathanRenderingAbstraction/Vendor/glad/"
)
# End rendering abstraction static library module target setup

# TODO: Build separate renderer modules that implement specific renderers (2D batch focused (sprite)/ 3D PBR etc...)

#------------------------------------------------------------------------------------------------
# Texture importer module
#------------------------------------------------------------------------------------------------

# Begin texture importer static library module target setup
# Add static library module target with source files to be compiled for all platforms
add_library("LeviathanTextureImporter" STATIC 
"Source/Engine/LeviathanTextureImporter/TextureImporter/TextureImporterModule.h"
"Source/Engine/LeviathanTextureImporter/TextureImporter/TextureImporterModule.cpp"
"Source/Engine/LeviathanTextureImporter/TextureImporter/TextureLoader.h"
"Source/Engine/LeviathanTextureImporter/TextureImporter/TextureLoader.cpp"
)

# Marked PUBLIC to share with executable target that is linking with the static library. PRIVATE only uses the precompiled header internally inside the library
target_precompile_headers("LeviathanTextureImporter" PUBLIC 
"Source/Engine/LeviathanTextureImporter/TextureImporter/TextureImporterPrecompiledHeader.h"
"Source/Engine/LeviathanCore/Public/CorePrecompiledHeader.h"
)

# Add static library module include directories
target_include_directories("LeviathanTextureImporter" PRIVATE 
"Source/Engine/LeviathanCore/Public/"
"Source/Engine/LeviathanTextureImporter/"
"Source/Engine/LeviathanTextureImporter/Vendor/stb_image/"
)
# End assets static library module target setup

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
"LeviathanRenderingAbstraction"
"LeviathanTextureImporter"
)

# Add engine static library module source directories as include directories for game title executable
target_include_directories("GameTitle" PRIVATE 
"Source/Engine/LeviathanCore/Public/"
"Source/Engine/LeviathanRenderingAbstraction/"
"Source/Engine/LeviathanTextureImporter/"
)
# End game title executable target setup

#################################################################################################
#################################################################################################

 # Set game title being built here
 set(TARGET_TITLE "GameTitle")

# Set Visual Studio startup project to target game title executable target
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${TARGET_TITLE}")

# Set working directory for game title being built to be the same as the game title target output directory
set_property(TARGET "${TARGET_TITLE}" PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:GameTitle>")

# Copy target title content to build target directory post target build
add_custom_command(TARGET "${TARGET_TITLE}" POST_BUILD COMMAND ${CMAKE_COMMAND} 
	-E copy_directory "${CMAKE_SOURCE_DIR}/GameTitleContent/" "$<TARGET_FILE_DIR:GameTitle>/GameTitleContent/"
)