
function(add_html IN OUT)
    add_custom_command(OUTPUT ${OUT}
        COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/top ${IN} ${CMAKE_CURRENT_SOURCE_DIR}/bottom1 > ${OUT}
        COMMAND date "+<div>Updated on %d-%m-%Y</div>" >> ${OUT}
        COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/bottom2 >> ${OUT}
        VERBATIM
        DEPENDS ${IN}
    )
endfunction()

add_html(${CMAKE_CURRENT_SOURCE_DIR}/docs.thtml docs.html)
add_html(${CMAKE_CURRENT_SOURCE_DIR}/index.thtml index.html)
add_html(${CMAKE_CURRENT_SOURCE_DIR}/plugins.thtml plugins.html)
add_html(${CMAKE_CURRENT_BINARY_DIR}/shots.thtml shots.html)

add_custom_command(OUTPUT shots.thtml
    COMMAND ./mk_gallery ${CMAKE_CURRENT_BINARY_DIR}/shots.thtml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shots
)

add_custom_target(www
    DEPENDS docs.html index.html plugins.html shots.html
)
