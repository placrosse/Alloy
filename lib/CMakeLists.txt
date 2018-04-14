cmake_minimum_required(VERSION 2.6)

if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-red-zone")
endif (CMAKE_COMPILER_IS_GNUCC)

add_library("alloy"
	"cmd.c"
	"color.c"
	"heap.c"
	"host.c"
	"input.c"
	"shell.c"
	"term.c"
	"theme.c")

function(ADD_ALLOY_TEST TEST)
	add_executable(${TEST} ${ARGN})
	target_link_libraries(${TEST} "alloy")
	add_test(${TEST} ${TEST})
endfunction(ADD_ALLOY_TEST TEST)

add_alloy_test("cmd-test" "cmd-test.c")

add_alloy_test("heap-test" "heap-test.c")

add_alloy_test("input-test" "input-test.c")

enable_testing()
