
function(add_mo NAME)
    add_custom_command(OUTPUT ${NAME}.mo
        COMMAND
            msgfmt ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.po --output-file ${NAME}.mo
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.po
    )
endfunction()

add_mo(fr_FR.UTF-8)
add_mo(ru_RU.UTF-8)
add_custom_target(po ALL
    DEPENDS
        fr_FR.UTF-8.mo ru_RU.UTF-8.mo
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/fr_FR.UTF-8.mo
    DESTINATION share/locale/fr/LC_MESSAGES
    RENAME ${PROJECT_NAME}.mo
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/ru_RU.UTF-8.mo
    DESTINATION share/locale/ru/LC_MESSAGES
    RENAME ${PROJECT_NAME}.mo
)
