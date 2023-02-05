cmake_minimum_required(VERSION 3.14)

project(labh LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(deps INTERFACE)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_sources(deps INTERFACE lib/OpenGL-windows.o)
	target_link_directories(deps INTERFACE lib/fltk-1.4-windows)
	target_compile_options(deps INTERFACE "-Wl,--subsystem,console" "-static")
	target_link_libraries(deps INTERFACE winmm glu32 opengl32 ole32 uuid comctl32 ws2_32 winpthread)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_sources(deps INTERFACE lib/OpenGL-macosx.o)
	target_link_directories(deps INTERFACE lib/fltk-1.4-macosx)
	target_include_directories(deps INTERFACE "/usr/include/freetype2" "/usr/X11/include")
	target_compile_options(deps INTERFACE -Wall -Wno-invalid-source-encoding -Wno-deprecated-declarations)
	target_link_libraries(deps INTERFACE "-framework Cocoa" "-framework AGL" "-framework OpenGL" "-framework ApplicationServices")
elseif(DEFINED UBUNTU)
	target_sources(deps INTERFACE lib/OpenGL-ubuntu.o)
	target_link_directories(deps INTERFACE lib/fltk-1.4-ubuntu)
	target_include_directories(deps INTERFACE "/usr/include/freetype2" "/usr/X11/include")
	target_compile_options(deps INTERFACE "-DUBUNTU")
	target_link_libraries(deps INTERFACE GLU GL Xext Xft Xrender fontconfig dl m X11 Xfixes)
else()
	target_sources(deps INTERFACE lib/OpenGL-linux.o)
	target_link_directories(deps INTERFACE lib/fltk-1.4-linux lib/fmod/4.3)
	target_include_directories(deps INTERFACE "lib/fmod/4.3" "/usr/include/freetype2" "/usr/X11/include")
	target_compile_options(deps INTERFACE "-Wl,--subsystem,console" "-static")
	target_link_libraries(deps INTERFACE GLU GL Xext Xft Xrender fontconfig dl m X11 Xfixes fmodex64)
endif()

add_subdirectory(lib/fmt-9.1.0)

set(SOURCES
	src/Character.cc
	src/Chasseur.cc
	src/Gardien.cc
	src/HUD.cc
	src/Labyrinthe.cc
	src/Position.cc
)

add_executable(labh ${SOURCES})
target_include_directories(labh PRIVATE include include/immutable)
target_link_libraries(labh PRIVATE deps fmt::fmt fltk_jpeg fltk_png fltk_z fltk_gl fltk)
target_compile_options(labh PRIVATE "-Wall")
