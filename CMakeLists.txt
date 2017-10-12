cmake_minimum_required(VERSION 2.6)

project("Alloy")

if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wfatal-errors")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib -nostartfiles -nodefaultlibs")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer -mno-red-zone")
endif (CMAKE_COMPILER_IS_GNUCC)

add_executable("alloy" "alloy.c")

target_link_libraries("alloy" "baremetal" "bmfs-library")

set_target_properties("alloy" PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/app.ld -static")
