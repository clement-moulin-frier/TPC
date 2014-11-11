# Copyright: (C) 2011 EFAA Consortium
# Authors: Jan Schneider
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

set(PROJECTNAME tpcLibrary)
project(${PROJECTNAME})
file(GLOB folder_header include/*.h )
file(GLOB folder_src src/*.cpp )
find_package(OpenCV REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${OpenCV_INCLUDE_DIRS})

add_library(${PROJECTNAME} ${folder_header} ${folder_src})
set_target_properties(${PROJECTNAME} PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "d")
target_link_libraries(${PROJECTNAME} ${OpenCV_LIBS})


#set (GLOB tpcLibrary_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${OPENCV_INCLUDE_DIRS})