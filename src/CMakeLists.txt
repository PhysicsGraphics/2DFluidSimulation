cmake_minimum_required(VERSION 2.8)

project(FluidSolver)
include_directories(../includes)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
FILE(GLOB HEADERS ${PROJECT_SOURCE_DIR}/../includes/*.h)
FILE(GLOB SHADERS ${PROJECT_SOURCE_DIR}/../shaders/*.vert)
FILE(GLOB SHADERS ${PROJECT_SOURCE_DIR}/../shaders/*.frag)
add_executable(FluidSolver ${SOURCES} ${HEADERS} ${SHADERS})


if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")



    set(LIBS "opengl32.lib" "glu32.lib"  ${PROJECT_SOURCE_DIR}/../thirdparties/freeglut/lib/freeglut.lib ${PROJECT_SOURCE_DIR}/../thirdparties/glew-1.13.0/lib/Release/Win32/glew32.lib)
    set(BUILD_TYPE STATIC)

	
target_link_libraries(FluidSolver ${LIBS})	
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
target_link_libraries(FluidSolver GL GLEW GLU freeglut)	
endif()




