cmake_minimum_required(VERSION 3.8)

project(tools VERSION 1.0)

include(GNUInstallDirs)

#add_subdirectory("tools")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -g")

message(${PROJECT_SOURCE_DIR})
message(${CMAKE_BINARY_DIR})

#add_library(tools STATIC
#    Thread.cpp
#    Times.cpp
#)

add_library(tools SHARED
	Thread.cpp
	Times.cpp
	Logger.cpp
	Queue.cpp
	Poll.cpp
)

#target_include_directories(videorender PRIVATE
#	${STAGING_INCDIR}/usr/include/
#	${STAGING_INCDIR}/usr/include/libdrm_meson
#	${STAGING_INCDIR}/usr/include/libdrm
#   ./tools
#	./
#)

include_directories(
    ./
)

add_executable(toolsbin 
    QueueSimple.cpp
    Thread.cpp
	Times.cpp
	Logger.cpp
	Queue.cpp)

target_link_libraries(toolsbin -lpthread -ldl stdc++)


