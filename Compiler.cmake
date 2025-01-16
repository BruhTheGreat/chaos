cmake_minimum_required(VERSION 3.12.4)

include_guard(GLOBAL)

find_program(C_COMPILER clang)
find_program(CXX_COMPILER clang++)
if (NOT C_COMPILER)
	message(FATAL_ERROR "clang not found")
elseif (NOT CXX_COMPILER)
	message(FATAL_ERROR "clang++ not found")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(FATAL_ERROR "MS Windows is currently unsupported")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	foreach(BREW /opt/homebrew;/usr/local/Homebrew)
		execute_process(COMMAND ${BREW}/bin/brew --prefix RESULT_VARIABLE BREW_RESULT OUTPUT_VARIABLE BREW_HOME OUTPUT_STRIP_TRAILING_WHITESPACE)
		if (BREW_RESULT EQUAL 0)
			break()
		endif()
	endforeach()
	if (NOT BREW_RESULT EQUAL 0 OR NOT IS_DIRECTORY ${BREW_HOME})
		message(FATAL_ERROR "Homebrew was not detected: ${BREW_RESULT}, exiting...")
	else()
		message(NOTICE "Homebrew was detected at ${BREW_HOME}")
	endif()
	set(BREW_HOME ${BREW_HOME}/opt)
endif ()

set(CMAKE_C_COMPILER "${C_COMPILER}" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${CXX_COMPILER}" CACHE STRING "C++ compiler" FORCE)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ Standard (toolchain)" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED YES CACHE BOOL "C++ Standard required" FORCE)
set(CMAKE_CXX_EXTENSIONS NO CACHE BOOL "C++ Standard extensions" FORCE)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "" FORCE)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
#set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -std=c++17" CACHE STRING "" FORCE)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_compile_definitions($<$<CONFIG:DEBUG>:DEBUG=1>)
add_compile_definitions($<$<CONFIG:DEBUG>:NDEBUG>)

add_compile_options(-Wall)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-unused-private-field)
add_compile_options($<$<CONFIG:DEBUG>:-g3>)
add_compile_options($<$<CONFIG:DEBUG>:-O0>)
add_compile_options($<$<CONFIG:RELEASE>:-O3>)
