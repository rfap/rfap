cmake_minimum_required (VERSION 3.0)
project (RFAP)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
	option(WITH_EPOLL "use epoll() to manage events" ON)
	option(WITH_PTHREAD "use PThread API" ON)
endif ()

set (librfapd_SOURCES )

if (WITH_EPOLL)
	set (CONFIG_EVMAN_EPOLL_AS_DEFAULT ON)
	list(APPEND librfapd_SOURCES src/evman_epoll.c)
endif ()

if (WITH_KQUEUE)
	list(APPEND librfapd_SOURCES src/evman_kqueue.c)
endif ()

if (WITH_PTHREAD)
	set (CONFIG_TASKQUEUE_PTHREAD ON)
endif ()

configure_file(include/rfap/config.h.in include/rfap/config.h)

list (APPEND librfapd_SOURCES src/server.c src/taskqueue.c)
add_library (librfapd SHARED ${librfapd_SOURCES})

set_target_properties (librfapd PROPERTIES OUTPUT_NAME rfapd)
add_executable (rfapd src/rfapd_main.c)

target_link_libraries (rfapd LINK_PUBLIC librfapd)

include_directories (${CMAKE_BINARY_DIR}/include include)
