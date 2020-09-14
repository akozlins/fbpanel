
configure_file(make_profile.in make_profile @ONLY)
configure_file(xlogout xlogout @ONLY)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/make_profile
        ${CMAKE_CURRENT_BINARY_DIR}/xlogout
    DESTINATION lib/fbpanel
# TODO: set executable
)
