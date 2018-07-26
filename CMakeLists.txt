cmake_minimum_required(VERSION 3.8)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(THIRD_PARTY_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/")

include(ExternalProject)
include(AMIOptions)


find_package(Git REQUIRED)

# Information from git
# ====================
include(AMIGit)

# AMI Project
# ============
project(AMI VERSION ${AMI_VERSION_MAJOR}.${AMI_VERSION_MINOR}.${AMI_VERSION_PATCH})
message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}")


# CCACHE
# ======
find_program(CCACHE_FOUND ccache)
message(STATUS "ccache: ${AMI_USE_CCACHE}")
if(CCACHE_FOUND AND AMI_USE_CCACHE)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# Set a default build type if none was specified
# =================================================
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Install path prefix prepended on to install directories." FORCE)
endif()


#configure_file(
#  "${CMAKE_CURRENT_SOURCE_DIR}/include/AMI/version.h.in"
#  "${CMAKE_CURRENT_BINARY_DIR}/include/AMI/version.h"
#)

set(AMI_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Manifest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Permission.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Activity.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Service.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BroadcastReceiver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/AndroidManifestImpl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ManifestReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/XML.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PrettyPrinter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/xml_search.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
)

set(AMI_PUBLIC_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/include
)

set(AMI_PRIVATE_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Create targets
# ==============
add_library(LIB_AMI_STATIC STATIC ${AMI_SOURCE_FILES}) # Static one
add_library(LIB_AMI_SHARED SHARED ${AMI_SOURCE_FILES}) # Shared one

# Include Directories
# ===================
target_include_directories(LIB_AMI_STATIC
  PUBLIC  "${AMI_PUBLIC_INCLUDE_DIR}"
  PRIVATE "${AMI_PRIVATE_INCLUDE_DIR}")

target_include_directories(LIB_AMI_SHARED
  PUBLIC  "${AMI_PUBLIC_INCLUDE_DIR}"
  PRIVATE "${AMI_PRIVATE_INCLUDE_DIR}")

target_compile_definitions(LIB_AMI_STATIC PRIVATE -DAMI_STATIC)
target_compile_definitions(LIB_AMI_SHARED PRIVATE -DAMI_EXPORTS)

# Flags definition
# ----------------
include(AMICompilerFlags)
set_property(TARGET LIB_AMI_STATIC LIB_AMI_SHARED PROPERTY CXX_STANDARD              11)
set_property(TARGET LIB_AMI_STATIC LIB_AMI_SHARED PROPERTY CXX_STANDARD_REQUIRED     ON)
set_property(TARGET LIB_AMI_STATIC LIB_AMI_SHARED PROPERTY POSITION_INDEPENDENT_CODE ON)

include(AMIDependencies)

# Output Name
# -----------
set_target_properties(
  LIB_AMI_STATIC
  PROPERTIES OUTPUT_NAME AMI
  CLEAN_DIRECT_OUTPUT 1)

set_target_properties(
  LIB_AMI_SHARED
  PROPERTIES OUTPUT_NAME AMI
  CLEAN_DIRECT_OUTPUT 1)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tools")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/bindings")
