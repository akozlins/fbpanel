
# generate list of plugins
set(PLUGINS "")
FILE(GLOB FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*
)
foreach(FILE ${FILES})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
        list(APPEND PLUGINS ${FILE})
    endif()
endforeach()

foreach(PLUGIN ${PLUGINS})

    file(GLOB PLUGIN_SOURCES
        ${PLUGIN}/*.c
    )

    add_library(${PLUGIN} SHARED ${PLUGIN_SOURCES})

    target_link_libraries(${PLUGIN} PRIVATE
        fbpanel
    )
    target_compile_options(${PLUGIN} PRIVATE
        -DPLUGIN
    )

endforeach()

add_custom_target(plugins
    ALL
    DEPENDS ${PLUGINS}
)

install(TARGETS ${PLUGINS}
    DESTINATION lib/fbpanel
)
