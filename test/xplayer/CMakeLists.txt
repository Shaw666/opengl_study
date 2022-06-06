cmake_minimum_required(VERSION 3.5)

project(xplayer VERSION 0.1 LANGUAGES CXX C)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(platform "64" CACHE STRING "")
else()
    set(platform "32" CACHE STRING "")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/Qt5.15.2/5.15.2/msvc2019_64")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
set(publibs "../../libs")
set(pubincludes "../../include")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

include_directories(${pubincludes})
link_directories(${publibs})


FILE(GLOB SRC_FILES ../../libs/glad/src/glad.c *.cpp *.h )
include_directories(${PUBLIBS}/include)
link_directories(${PUBLIBS}/libs)
include_directories(../../libs/glad/include)

set(PROJECT_SOURCES
    ${SRC_FILES}
)

add_executable(xplayer WIN32
    ${PROJECT_SOURCES}
)

target_link_libraries(xplayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets opengl32)

set_target_properties(xplayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(WIN32)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/${platform})
else()
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/${platform}/${CMAKE_BUILD_TYPE})
endif()