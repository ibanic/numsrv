# for linux ubuntu 18 use gcc 8
# sudo python3 setup.py develop -DCMAKE_C_COMPILER=gcc-8 -DCMAKE_CXX_COMPILER=g++-8




cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(_NumSrvHelper)
# Set source directory
set(SOURCE_DIR "src")
# Tell CMake that headers are also in SOURCE_DIR
#include_directories(${SOURCE_DIR})
#set(SOURCES "${SOURCE_DIR}/math.cpp")


file(GLOB Sources "${SOURCE_DIR}/*.cpp")
file(GLOB SourcesC "${SOURCE_DIR}/*.c")
file(GLOB HeadersC "${SOURCE_DIR}/*.h")
file(GLOB Headers "${SOURCE_DIR}/*.hpp")


message("++++++++ build: ${CMAKE_BUILD_TYPE} ++++++++")


add_library(${PROJECT_NAME} MODULE ${Sources} ${HeadersC} ${HeadersC} ${Headers})



# ICU
#
# command line add -DCMAKE_PREFIX_PATH=/usr/local/opt/icu4c
# GUI add entry:    CMAKE_PREFIX_PATH     /usr/local/opt/icu4c
#





add_subdirectory("./search" "search")
INCLUDE_DIRECTORIES("./search")
INCLUDE_DIRECTORIES("./search/tessil-hopscotch-map/include")
#target_link_libraries(${PROJECT_NAME} Search )
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)





#### Boost ####
FIND_PACKAGE( Boost 1.60 COMPONENTS filesystem python iostreams REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${Boost_LIBRARIES} )







#### Python ####
find_package(PythonInterp 3.5 REQUIRED)
find_package( PythonLibs REQUIRED )
include_directories ( ${PYTHON_INCLUDE_DIRS} )
#target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})


#### Threads, Filesystem ####
FIND_PACKAGE(Threads)



set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}")
#set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}")



set(LibrariesToLink ${Boost_LIBRARIES} ${CITYHASH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} "Search")

#### Filesystem ####
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND LibrariesToLink "stdc++fs")
endif()



#if(WIN32 OR CYGWIN)
#    target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARIES}  ${LibrariesToLink})
#else()
#    target_link_libraries(${PROJECT_NAME} PRIVATE ${LibrariesToLink} "-undefined dynamic_lookup")
#endif()



target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIBRARIES}  ${LibrariesToLink})





