
function(add_html IN OUT)
    add_custom_command(OUTPUT ${OUT}
        COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/top ${CMAKE_CURRENT_SOURCE_DIR}/${IN} ${CMAKE_CURRENT_SOURCE_DIR}/bottom1 > ${OUT}
        COMMAND date "+<div>Updated on %d-%m-%Y</div>" >> ${OUT}
        COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/bottom2 >> ${OUT}
        VERBATIM
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${IN}
    )
endfunction()

add_html(docs.thtml docs.html)
add_html(index.thtml index.html)
add_html(plugins.thtml plugins.html)
add_html(shots.thtml shots.html)

add_custom_target(www
    ALL
    DEPENDS docs.html index.html plugins.html shots.html
)
