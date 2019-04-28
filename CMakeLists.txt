set(CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER OFF CACHE BOOL "Include stack walker in Cafe.ErrorHandling")
set(CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION OFF CACHE BOOL
    "Enable stack walker in Cafe::Exception, available only if CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER is enabled")

set(SOURCE_FILES
    src/ErrorHandling.cpp)

set(HEADERS
    include/Cafe/ErrorHandling/ErrorHandling.h)

if(CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER)
    list(APPEND SOURCE_FILES src/StackWalker.cpp)
    list(APPEND HEADERS include/Cafe/ErrorHandling/StackWalker.h)
endif()

add_library(Cafe.ErrorHandling ${SOURCE_FILES} ${HEADERS})

configure_file(cmake/ErrorHandlingConfig.h.in Cafe/ErrorHandling/Config/ErrorHandlingConfig.h)

target_include_directories(Cafe.ErrorHandling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(Cafe.ErrorHandling PUBLIC Cafe.Encoding.UTF-8)

if(CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER)
    target_link_libraries(Cafe.ErrorHandling PRIVATE Cafe.TextUtils.Misc)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        message(STATUS "Including stack walker introduces dependency of DbgHelp.lib")
        target_link_libraries(Cafe.ErrorHandling PRIVATE DbgHelp.lib)
    endif()
endif()

install(TARGETS Cafe.ErrorHandling
    EXPORT ErrorHandling)

install(DIRECTORY include/
    TYPE INCLUDE
    FILES_MATCHING PATTERN "*.h")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Cafe/ErrorHandling/Config/ErrorHandlingConfig.h
    DESTINATION include/Cafe/ErrorHandling/Config)

install(EXPORT ErrorHandling
    DESTINATION cmake/ErrorHandling)
