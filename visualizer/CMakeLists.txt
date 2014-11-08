cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

SET(PROJECT visializer)
project(${PROJECT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

find_package(OpenGL REQUIRED)

find_package(Qt4 COMPONENTS QtOpenGL QtCore QtGui REQUIRED)
include(${QT_USE_FILE})

include_directories(3rdparty/include )

add_subdirectory(3rdparty)

SET(HEADERS line.h)
SET(SOURCES viewer.cpp)

add_executable(${PROJECT} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT} ${QT_LIBRARIES} visualization)
