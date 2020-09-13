cmake_minimum_required(VERSION 3.8)

project(fbpanel VERSION 7.0)

configure_file(config.h.in ${CMAKE_BINARY_DIR}/include/config.h @ONLY)

file(GLOB FBPANEL_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    panel/*.h panel/*.c
)

add_executable(fbpanel ${FBPANEL_SOURCES})
set_target_properties(fbpanel PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ENABLE_EXPORTS TRUE
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODULES REQUIRED x11 gtk+-2.0 gmodule-2.0)

target_include_directories(fbpanel PUBLIC
    ${CMAKE_BINARY_DIR}/include
    panel .
)
target_link_libraries(fbpanel PUBLIC
    -lm
    ${MODULES_LIBRARIES}
)
target_include_directories(fbpanel SYSTEM PUBLIC
    ${MODULES_INCLUDE_DIRS}
)

set(PLUGINS "")
FILE(GLOB FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/*
)
foreach(FILE ${FILES})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/plugins/${FILE})
        list(APPEND PLUGINS ${FILE})
    endif()
endforeach()

foreach(PLUGIN ${PLUGINS})

    file(GLOB PLUGIN_SOURCES
        plugins/${PLUGIN}/*.c
    )

    add_library(${PLUGIN} SHARED ${PLUGIN_SOURCES})
    set_target_properties(${PLUGIN} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/fbpanel"
    )

    target_link_libraries(${PLUGIN} PRIVATE
        fbpanel
    )
    target_compile_options(${PLUGIN} PRIVATE
        -DPLUGIN
    )

endforeach()

install(TARGETS fbpanel
    DESTINATION bin
)

install(TARGETS ${PLUGINS}
    DESTINATION lib/fbpanel
)

set(datadir "${CMAKE_INSTALL_PREFIX}/share/fbpanel")
configure_file(exec/make_profile.in ${CMAKE_BINARY_DIR}/libexec/fbpanel/make_profile @ONLY)
configure_file(exec/xlogout ${CMAKE_BINARY_DIR}/libexec/fbpanel/xlogout @ONLY)
install(
    DIRECTORY ${CMAKE_BINARY_DIR}/libexec/fbpanel
    DESTINATION libexec
# TODO: set executable
)

set(DATADIR "${CMAKE_INSTALL_PREFIX}/share/fbpanel")
configure_file(data/man/fbpanel.1.in ${CMAKE_BINARY_DIR}/man/fbpanel.1 @ONLY)
install(
    FILES ${CMAKE_BINARY_DIR}/man/fbpanel.1
    DESTINATION man
)

configure_file(data/config/default.in ${CMAKE_BINARY_DIR}/share/fbpanel/default @ONLY)
configure_file(data/config/pager.in ${CMAKE_BINARY_DIR}/share/fbpanel/pager @ONLY)
file(COPY data/images DESTINATION ${CMAKE_BINARY_DIR}/share/fbpanel)
install(
    DIRECTORY ${CMAKE_BINARY_DIR}/share/fbpanel
    DESTINATION share
)
