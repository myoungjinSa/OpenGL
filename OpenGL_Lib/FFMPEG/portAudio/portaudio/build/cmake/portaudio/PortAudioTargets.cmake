# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.19)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget PortAudio)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target PortAudio
add_library(PortAudio SHARED IMPORTED)

set_target_properties(PortAudio PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/include;D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/src/common"
)

# Import target "PortAudio" for configuration "Debug"
set_property(TARGET PortAudio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PortAudio PROPERTIES
  IMPORTED_IMPLIB_DEBUG "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/Debug/portaudio.lib"
  IMPORTED_LOCATION_DEBUG "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/Debug/portaudio.dll"
  )

# Import target "PortAudio" for configuration "Release"
set_property(TARGET PortAudio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PortAudio PROPERTIES
  IMPORTED_IMPLIB_RELEASE "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/Release/portaudio.lib"
  IMPORTED_LOCATION_RELEASE "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/Release/portaudio.dll"
  )

# Import target "PortAudio" for configuration "MinSizeRel"
set_property(TARGET PortAudio APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(PortAudio PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/MinSizeRel/portaudio.lib"
  IMPORTED_LOCATION_MINSIZEREL "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/MinSizeRel/portaudio.dll"
  )

# Import target "PortAudio" for configuration "RelWithDebInfo"
set_property(TARGET PortAudio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(PortAudio PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/RelWithDebInfo/portaudio.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "D:/FFMPEG/VideoApp/VideoApp/lib/portAudio/portaudio/build/RelWithDebInfo/portaudio.dll"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
