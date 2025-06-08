cmake_minimum_required(VERSION 3.17)

project(fbpanel VERSION 7.0)

include(GNUInstallDirs)

set(LIBDIR "${CMAKE_INSTALL_PREFIX}/lib/fbpanel")
set(DATADIR "${CMAKE_INSTALL_PREFIX}/share/fbpanel")

add_subdirectory(data)
add_subdirectory(exec)
add_subdirectory(panel)
add_subdirectory(plugins)
add_subdirectory(po)
