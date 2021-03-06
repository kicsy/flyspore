message(STATUS "--------CMAKE--------")
cmake_minimum_required(VERSION 3.10.0)
message(STATUS "--------PROJECT--------")
project(flyspore VERSION 0.0.1 LANGUAGES CXX)
message(STATUS "----------------")
#include(${CMAKE_SOURCE_DIR}/cmake/base.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)


message(STATUS "PROJECT_SOURCE_DIR:" ${PROJECT_SOURCE_DIR})
message(STATUS "PROJECT_BINARY_DIR:" ${PROJECT_BINARY_DIR})
message(STATUS "CMAKE_HOST_SYSTEM:" ${CMAKE_HOST_SYSTEM})
message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})
message(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})
message(STATUS "PROJECT_NAME: " ${PROJECT_NAME})



# Compiler vendors
message(STATUS "--------COMPILER INFO--------")
message(STATUS "CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS "CMAKE_CONFIGURATION_TYPES: " ${CMAKE_CONFIGURATION_TYPES})
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
message(STATUS "------------------------")
## g++
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.3")
    message(FATAL_ERROR "\nrequires g++ at least v7.3")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
## clang++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
    message(FATAL_ERROR "\nrequires clang++ at least v6.0")
  endif() 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
## microsoft visual c++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(NOT MSVC_VERSION GREATER_EQUAL 1900)
    message(FATAL_ERROR "\nrequires MSVC++ at least v14.00") 
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W3")
  add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
else()
  message(FATAL_ERROR "\n\
currently supports the following compilers:\n\
- g++ v7.3 or above\n\
- clang++ v6.0 or above\n\
- MSVC++ v19.14 or above\n\
")
endif()

##add_subdirectory(3rdparty)

add_subdirectory(core)

add_subdirectory(flyspore)


