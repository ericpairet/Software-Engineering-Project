project(seed_based_segmentation)
cmake_minimum_required(VERSION 2.8.11)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt5 :
set(qt5_modules
    #Qt5Core
    #Qt5Gui
    Qt5Widgets
)

foreach(qt5_module ${qt5_modules})
    find_package(${qt5_module} REQUIRED)
    include_directories(${${qt5_module}_INCLUDE_DIRS})
    add_definitions(${${qt5_module}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5_module}_EXECUTABLE_COMPILE_FLAGS}")
    list(APPEND QT_LIBRARIES ${${qt5_module}_LIBRARIES})
endforeach()

#Eigen :
find_package(Eigen REQUIRED)
include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})
if (NOT EIGEN_INCLUDE_DIR OR
    NOT EXISTS ${EIGEN_INCLUDE_DIR})
  include(cmake/FindEigen.cmake               REQUIRED)
endif (NOT EIGEN_INCLUDE_DIR OR
       NOT EXISTS ${EIGEN_INCLUDE_DIR})

#GoogleTest :
find_package(GTest REQUIRED)
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

# Source Directory :
include_directories(${PROJECT_SOURCE_DIR}/src/app)

set (SRCS
	src/mainwindow.cpp
	src/widgets.cpp
	src/tester.cpp
	src/segmentation.cpp
	src/main.cpp
)

set (arch ${CMAKE_SYSTEM_PROCESSOR})
message( STATUS "CMAKE_SYSTEM_VERSION=          ${CMAKE_SYSTEM_VERSION}")
message( STATUS "CMAKE_SYSTEM_PROCESSOR=        ${CMAKE_SYSTEM_PROCESSOR}" )
message( STATUS "EXECUTABLE_OUTPUT_PATH=        ${EXECUTABLE_OUTPUT_PATH}" )
message (STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message (STATUS "Arch: ${arch}")

if (${arch} MATCHES "x86_64")
	#set compiler flags here for x86_64
	set (cc_flags "")
elseif (${arch} MATCHES "x86")
	#set compiler flags here for x86
	set (cc_flags "")
endif()

ADD_DEFINITIONS(
    -std=c++11
    -lgtest 
    -lpthread
)

#flags to set in debug mode
set (CMAKE_CXX_FLAGS_DEBUG "-g -Wl,--no-as-needed")

#flags to set in release mode
set (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -march=native -Wl,--no-as-needed")

message( STATUS "CC Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
message (STATUS "Custom CC Flags: ${cc_flags}")

add_definitions(${cc_flags})

set (libs ${QT_LIBRARIES} opencv_core opencv_imgproc pthread ${GTEST_BOTH_LIBRARIES}) 

## build the main app
set (target seedBasedSegmentation)
add_executable(${target} ${MOC_SRCS} ${SRCS})
target_link_libraries(${target} ${libs})
