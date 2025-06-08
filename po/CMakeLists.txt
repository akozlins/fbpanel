#

find_program(XGETTEXT xgettext)
if(NOT XGETTEXT)
    return()
endif()

find_program(MSGMERGE msgmerge)
if(NOT MSGMERGE)
    return()
endif()

find_program(MSGFMT msgfmt)
if(NOT MSGFMT)
    return()
endif()

# generate .pot files from *.c and *.h files
function(add_pot NAME)
    file(GLOB_RECURSE SOURCES
        ../*.c ../*.h
    )
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pot
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND
            find . -name "*.c" -o -name "*.h" | sort |
            xgettext --package-name=${PROJECT_NAME} --package-version=${PROJECT_VERSION} --default-domain=${PROJECT_NAME}
                --from-code=UTF-8 --force-po
                --keyword=_ --keyword=c_
                --files-from=- --output=- |
            sed "/^\\\"Content-Type:/ s/CHARSET/UTF-8/" > ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pot
        DEPENDS ${SOURCES}
    )
endfunction()

# merge current .po and .pot files
function(add_po NAME)
    add_custom_command(OUTPUT ${NAME}.po
        COMMAND
            msgmerge ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.po ${NAME}.pot
                > ${NAME}.po
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.po ${NAME}.pot
    )
endfunction()

# generate .mo files
function(add_mo NAME)
    add_custom_command(OUTPUT ${NAME}.mo
        COMMAND
            msgfmt ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.po --output-file ${NAME}.mo
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.po
    )
endfunction()

add_pot(fr_FR.UTF-8)
add_pot(ru_RU.UTF-8)

add_po(fr_FR.UTF-8)
add_po(ru_RU.UTF-8)

add_mo(fr_FR.UTF-8)
add_mo(ru_RU.UTF-8)

add_custom_target(po ALL
    DEPENDS
        fr_FR.UTF-8.po ru_RU.UTF-8.po
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
