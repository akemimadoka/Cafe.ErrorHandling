cmake_minimum_required(VERSION 3.15)

project(Cafe.Environment CXX)

set(CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER OFF CACHE BOOL "Include stack walker in Cafe.ErrorHandling")
set(CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION ON CACHE BOOL
    "Enable stack walker in Cafe::ErrorHandling::CafeException, available only if CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER is enabled")

list(APPEND CAFE_OPTIONS
    CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER
    CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION
)

include(${CMAKE_CURRENT_SOURCE_DIR}/CafeCommon/cmake/CafeCommon.cmake)

configure_file(cmake/ErrorHandlingConfig.h.in Cafe/ErrorHandling/Config/ErrorHandlingConfig.h)

set(SOURCE_FILES
    src/Cafe/ErrorHandling/CommonExceptions.cpp
    src/Cafe/ErrorHandling/ErrorHandling.cpp)

set(HEADERS
    src/Cafe/ErrorHandling/CommonExceptions.h
    src/Cafe/ErrorHandling/ErrorHandling.h)

if(CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER)
    list(APPEND SOURCE_FILES src/Cafe/ErrorHandling/StackWalker.cpp)
    list(APPEND HEADERS src/Cafe/ErrorHandling/StackWalker.h)
endif()

add_library(Cafe.ErrorHandling ${SOURCE_FILES} ${HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/Cafe/ErrorHandling/Config/ErrorHandlingConfig.h)

target_compile_features(Cafe.ErrorHandling PUBLIC cxx_std_20)

target_include_directories(Cafe.ErrorHandling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(Cafe.ErrorHandling
    PUBLIC CONAN_PKG::Cafe.Encoding)

AddCafeSharedFlags(Cafe.ErrorHandling)

if(CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        message(STATUS "Including stack walker introduces dependency of DbgHelp.lib")
        target_link_libraries(Cafe.ErrorHandling PUBLIC DbgHelp.lib)
    endif()
endif()

install(TARGETS Cafe.ErrorHandling
    EXPORT ErrorHandling)

install(DIRECTORY src/
    TYPE INCLUDE
    FILES_MATCHING PATTERN "*.h")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Cafe/ErrorHandling/Config/ErrorHandlingConfig.h
    DESTINATION include/Cafe/ErrorHandling/Config)

install(EXPORT ErrorHandling
    DESTINATION cmake/ErrorHandling)
