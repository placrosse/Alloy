cmake_minimum_required(VERSION 2.6)

if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-red-zone")
endif (CMAKE_COMPILER_IS_GNUCC)

add_library("alloy"
	"app.c"
	"app-bin.c"
	"app-elf.c"
	"cmd.c"
	"color.c"
	"heap.c"
	"host.c"
	"input.c"
	"scanner.c"
	"scheme.c"
	"shell.c"
	"string.c"
	"term.c"
	"var.c")

function(ADD_ALLOY_TEST TEST)
	add_executable(${TEST} ${ARGN})
	target_link_libraries(${TEST} "alloy")
	add_test(${TEST} ${TEST})
endfunction(ADD_ALLOY_TEST TEST)

add_alloy_test("cmd-test" "cmd-test.c")

add_alloy_test("heap-test" "heap-test.c")

add_alloy_test("input-test" "input-test.c")

add_alloy_test("scanner-test" "scanner-test.c")

add_alloy_test("var-test" "var-test.c")

enable_testing()
