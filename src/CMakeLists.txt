cmake_minimum_required(VERSION 2.6)

set (SOURCES "alloy.c")

if (UNIX)
	list (APPEND SOURCES "xterm.c" "host-linux.c")
endif (UNIX)

add_executable("alloy-cli" ${SOURCES})

set_target_properties("alloy-cli" PROPERTIES OUTPUT_NAME "alloy")

target_link_libraries("alloy-cli" "alloy")
