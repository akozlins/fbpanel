
file(GLOB SOURCES
    *.h *.c
)

configure_file(config.h.in config.h @ONLY)

add_executable(fbpanel ${SOURCES})
set_target_properties(fbpanel PROPERTIES
    ENABLE_EXPORTS TRUE
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODULES REQUIRED x11 gtk+-2.0 gmodule-2.0)

target_include_directories(fbpanel PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    .
)
target_link_libraries(fbpanel PUBLIC
    -lm
    ${MODULES_LIBRARIES}
)
target_include_directories(fbpanel SYSTEM PUBLIC
    ${MODULES_INCLUDE_DIRS}
)

install(TARGETS fbpanel
    DESTINATION bin
)
