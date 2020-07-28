cmake_minimum_required(VERSION 3.8)

project(fbpanel VERSION 7.0)

configure_file(
    config.h.in ${CMAKE_BINARY_DIR}/include/config.h
)

file(GLOB FBPANEL_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    panel/*.h panel/*.c
)

add_executable(fbpanel ${FBPANEL_SOURCES})
set_target_properties(fbpanel PROPERTIES
    LINKER_LANGUAGE C
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

find_package(X11 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODULES REQUIRED gtk+-2.0 gmodule-2.0)

target_include_directories(fbpanel
    PUBLIC
        ${CMAKE_BINARY_DIR}/include
        panel .
)
target_link_libraries(fbpanel
    PRIVATE
        -lm
        ${X11_LIBRARIES}
        ${MODULES_LIBRARIES}
)
target_include_directories(fbpanel
    SYSTEM PRIVATE
        ${X11_INCLUDE_DIRS}
        ${MODULES_INCLUDE_DIRS}
)

set(PLUGINS "")
FILE(GLOB FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    plugins/*
)
foreach(FILE ${FILES})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/plugins/${FILE})
        list(APPEND PLUGINS ${FILE})
    endif()
endforeach()

foreach(PLUGIN ${PLUGINS})

    file(GLOB PLUGIN_SOURCES
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        plugins/${PLUGIN}/*.c
    )

    add_library(${PLUGIN} SHARED ${PLUGIN_SOURCES})
    set_target_properties(${PLUGIN} PROPERTIES
        LINKER_LANGUAGE C
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/fbpanel"
    )

    target_include_directories(${PLUGIN}
        PUBLIC
            ${CMAKE_BINARY_DIR}/include
            panel .
    )
    target_link_libraries(${PLUGIN}
        PRIVATE
            ${MODULES_LIBRARIES}
    )
    target_include_directories(${PLUGIN}
        SYSTEM PRIVATE
            ${MODULES_INCLUDE_DIRS}
    )

endforeach()

install(
    TARGETS fbpanel
    DESTINATION bin/
)

install(
    TARGETS ${PLUGINS}
    DESTINATION lib/fbpanel/
)

file(COPY data/images DESTINATION ${CMAKE_BINARY_DIR}/share/fbpanel)
install(
    DIRECTORY ${CMAKE_BINARY_DIR}/share/fbpanel
    DESTINATION share
)
