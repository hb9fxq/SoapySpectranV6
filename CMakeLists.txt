###################################################
# Build Soapy SDR support module for SpectranV6 Devices
###################################################

cmake_minimum_required(VERSION 2.8.7)

project(SoapySpectranV6 CXX)

find_package(SoapySDR "0.4.0" NO_MODULE REQUIRED)
if (NOT SoapySDR_FOUND)
    message(FATAL_ERROR "Soapy SDR development files not found...")
endif ()

add_library( spectranstream SHARED IMPORTED)
set_property(TARGET spectranstream PROPERTY IMPORTED_LOCATION "${SPECTRANSTREAM_LIBRARY}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${spectranstream_SOURCE_DIR})

#enable c++11 features
if(CMAKE_COMPILER_IS_GNUCXX)
    #C++11 is a required language feature for this project
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_STD_CXX11)
    if(HAS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else(HAS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
    #Thread support enabled (not the same as -lpthread)
    list(APPEND spectranstream -pthread)
    #disable warnings for unused parameters
    add_definitions(-Wno-unused-parameter)
endif(CMAKE_COMPILER_IS_GNUCXX)

SOAPY_SDR_MODULE_UTIL(
    TARGET SpectranV6Support
    SOURCES
        SoapySpectranV6.hpp
        Registration.cpp
        Settings.cpp
        Streaming.cpp
    LIBRARIES
        spectranstream
)
