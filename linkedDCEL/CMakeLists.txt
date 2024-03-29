cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

SET(PROJECT linkedDCEL)
project(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

find_package(OpenGL REQUIRED)
find_package(GMP REQUIRED)


find_package(Qt4 COMPONENTS QtOpenGL QtCore QtGui REQUIRED)
include(${QT_USE_FILE})


include_directories(3rdparty/include)

add_subdirectory(3rdparty)


SET(HEADERS
line.h
dcel.h
linked_infinite_dcel.h
linked_triangle_dcel.h
vertex.h
edge.h
face.h
orientation.h
orientation3d.h
line_wrapper.h
triangle.h
kirkpatrick.h
)

SET(SOURCES
viewer.cpp
edge.cpp
face.cpp
triangle.cpp
dcel_triangulation.cpp
dcel_checker.cpp
)

add_executable(${PROJECT} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT} ${QT_LIBRARIES} visualization)
target_link_libraries(${PROJECT} ${GMP_LIBRARIES})
