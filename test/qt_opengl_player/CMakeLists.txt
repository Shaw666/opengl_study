cmake_minimum_required(VERSION 3.5)
project(qt_opengl_player VERSION 0.1 LANGUAGES CXX)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(platform "64" CACHE STRING "")
else()
    set(platform "32" CACHE STRING "")
endif()
# get_filename_component(ABSOLUTE_PATH "C:/work/publibs" ABSOLUTE)
get_filename_component(ABSOLUTE_PATH "E:/work/publibs" ABSOLUTE)
set(PUBLIBS "${ABSOLUTE_PATH}/${platform}" CACHE PATH "")

set(CMAKE_INSTALL_PREFIX ${PUBLIBS})
# set(QT_DIR "C:/Qt/Qt5.15.2/5.15.2/msvc2019_64" CACHE PATH "")
set(QT_DIR "E:/Qt/5.15.2/msvc2019_64" CACHE PATH "")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT_DIR}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(RC_FILES mainwindow.ui)
FILE(GLOB PROJECT_SOURCES *.cpp *.h)

# include_directories(${PUBLIBS}/libportaudio/include)
# link_directories(${PUBLIBS}/libportaudio/lib)
include_directories(${PUBLIBS}/libffmpeg/include)
link_directories(${PUBLIBS}/libffmpeg/lib)

message("${PUBLIBS} ${CMAKE_SIZEOF_VOID_P}")

add_executable(qt_opengl_player ${PROJECT_SOURCES}  ${RC_FILES})
target_link_libraries(qt_opengl_player PRIVATE Qt${QT_VERSION_MAJOR}::Widgets avcodec avformat avutil avfilter swscale d3d9)

SET(RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/${platform})
SET(LIBRARY_OUTPUT_PATH      ${CMAKE_SOURCE_DIR}/${platform})