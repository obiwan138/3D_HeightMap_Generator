# CMake entry point
cmake_minimum_required (VERSION 3.0)

# Define the project 
project (ECE4122-FP)

############################################### 
# Add the necessary dependencies
###############################################

# OpenGL
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

# Debug
if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

# Distributions
if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)


############################################### 
# Select the directories to compile
###############################################

include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
	external/SFML/include
	./
  include/
  src/
)

# Link the executable to the SFML libraries
link_directories(external/SFML/lib)

# Name the libraries
set(ALL_LIBS
	${OPENGL_LIBRARY}
	${Boost_LIBRARIES}
	glfw
	GLEW_1130
	sfml-graphics 
	sfml-system 
	sfml-window
)

# Libraries definitions
add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

############################################### 
# Select the sources to compile
###############################################

# Define sources
file(GLOB SOURCES src/*.cpp)

# main
add_executable(main
  	${SOURCES}								# .cpp source files in /src
	common/shader.cpp						# Wrapper to load and compile shaders (header)
	common/shader.hpp						# Wrapper to load and compile shaders
	src/StandardShading.vertexshader		# Vertex shader
	src/StandardShading.fragmentshader		# Fragment shader
)



# Link the libraries to the target
target_link_libraries(main					# Target executable 
	${ALL_LIBS}								
)

# Xcode and Visual working directories
set_target_properties(main PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
create_target_launcher(main WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/")


SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET main POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/main${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

if (BUILD_TEST)
    add_subdirectory (test)
endif (BUILD_TEST)
