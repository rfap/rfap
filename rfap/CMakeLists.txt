cmake_minimum_required (VERSION 3.0)
project (RFAP)

if (CMAKE_SYSTEM_NAME EQUAL Linux)
	option(WITH_EPOLL "use epoll to manage events" ON)
endif ()

set (librfapd_SOURCES src/server.c)
if (WITH_EPOLL)
	list(APPEND librfapd_SOURCES src/evman_epoll.c)
endif ()
add_library (librfapd SHARED ${librfapd_SOURCES})
set_target_properties (librfapd PROPERTIES OUTPUT_NAME rfapd)
add_executable (rfapd src/rfapd_main.c)
target_link_libraries (rfapd LINK_PUBLIC librfapd)

include_directories (include)
