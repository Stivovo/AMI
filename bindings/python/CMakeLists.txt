cmake_minimum_required(VERSION 3.1)
include(ExternalProject)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

set(PYBIND11_VERSION 2.2.1)
set(PYBIND11_SHA256 SHA256=53c373d7d0b0711fea96beba666a9985269dc9e43e6088ea73faaa89b07b972e)
set(PYBIND11_URL "${THIRD_PARTY_DIRECTORY}/pybind11-${PYBIND11_VERSION}.zip" CACHE STRING "URL to the Pybind11 repo")
ExternalProject_Add(ami_pybind11
  URL               ${PYBIND11_URL}
  URL_HASH          ${PYBIND11_SHA256}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   "")
ExternalProject_get_property(ami_pybind11 SOURCE_DIR)
set(PYBIND11_SOURCE_DIR "${SOURCE_DIR}")

# Add a CMake parameter for choosing a desired Python version
if(NOT AMI_PYTHON_VERSION)
  set(AMI_PYTHON_VERSION "" CACHE STRING "Python version to use for compiling modules")
endif()

set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
find_package(PythonLibsNew ${AMI_PYTHON_VERSION} REQUIRED)

set(AMI_PYTHON_SRC
  pyAMI.cpp
  pyXML.cpp
)

add_library(pyAMI SHARED ${AMI_PYTHON_SRC})

set_property(TARGET pyAMI PROPERTY CXX_STANDARD          11)
set_property(TARGET pyAMI PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(pyAMI PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/"
  "${PYTHON_INCLUDE_DIR}"
  "${PYBIND11_SOURCE_DIR}/include")


if (MSVC)
  target_compile_options(pyAMI PUBLIC /FIiso646.h)
	set_property(TARGET pyAMI PROPERTY LINK_FLAGS /NODEFAULTLIB:MSVCRT)
endif()

#set_target_properties(pyAMI PROPERTIES PYTHON_VERSION ${PYTHON_VERSION})
#set_target_properties(pyAMI PROPERTIES PREFIX "" OUTPUT_NAME "pyami")
add_dependencies(pyAMI ami_pybind11)
add_dependencies(pyAMI LIB_AMI_STATIC)

target_link_libraries(pyAMI PUBLIC LIB_AMI_STATIC)


if(APPLE)
    set_target_properties(pyAMI PROPERTIES MACOSX_RPATH ".")
    set_target_properties(pyAMI PROPERTIES LINK_FLAGS "-undefined dynamic_lookup ")
endif()

set_target_properties(pyAMI PROPERTIES PREFIX "")
if (UNIX)
  set_target_properties(pyAMI PROPERTIES SUFFIX ".so")
elseif(WIN32)
  set_target_properties(pyAMI PROPERTIES SUFFIX ".pyd")
endif()
