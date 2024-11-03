# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (ECE4122-FP)

find_package(Boost REQUIRED COMPONENTS system iostreams filesystem program_options)

include_directories(
	../external/glm-0.9.7.1/
	../external/gnuplot-iostream
    ../external/CmdParser
	../include/
    ${Boost_INCLUDE_DIRS}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Tutorial 9
add_executable(perlin-test
    perlin-test.cpp
    ../src/Perlin.cpp
)

target_link_libraries(perlin-test
    ${Boost_LIBRARIES}
)
