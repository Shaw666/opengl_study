cmake_minimum_required (VERSION 3.20)

STRING(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set (TARGET_NAME ${CURRENT_FOLDER})
# message("${TARGET_NAME}": ===================)
project(${TARGET_NAME} VERSION 0.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SRC_FILES *.cpp *.h ../glad/src/glad.c)
set(PUBLIBS "E:/opengl/publibs/")
#PATHS "E:/opengl/publibs/libglfw/lib/cmake/glfw3"
# find_package(GLFW glfw3 PATHS "E:/opengl/publibs/libglfw"  REQUIRED)
include_directories(${PUBLIBS}/libglfw/include)
link_directories(${PUBLIBS}/libglfw/lib)
include_directories(../glad/include)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw3 opengl32)


SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})


