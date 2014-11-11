# Copyright: (C) 2011 EFAA Consortium
# Authors: Jan Schneider
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

add_subdirectory(tpcLibrary)

set(PROJECTNAME tpcStandalone)
project(${PROJECTNAME})
find_package(OpenCV REQUIRED)
file(GLOB folder_source *.h *.cpp)
source_group("Source Files" FILES ${folder_source})

include_directories(${tpcLibrary_INCLUDE_DIRS})
include_directories(tpcLibrary/include ${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECTNAME} ${folder_source})
set_target_properties(${PROJECTNAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECTNAME} tpcLibrary  ${OpenCV_LIBS})
