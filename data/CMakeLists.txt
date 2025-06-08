#

configure_file(config/default.in config/default @ONLY)
configure_file(config/pager.in config/pager @ONLY)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/config/default
        ${CMAKE_CURRENT_BINARY_DIR}/config/pager
    DESTINATION share/${PROJECT_NAME}
)

install(
    DIRECTORY images
    DESTINATION share/${PROJECT_NAME}
)

configure_file(man/fbpanel.1.in man/fbpanel.1 @ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/man/fbpanel.1
    DESTINATION man/man1
)
