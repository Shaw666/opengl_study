cmake_minimum_required (VERSION 3.20)

STRING(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set (TARGET_NAME ${CURRENT_FOLDER})
# message("${TARGET_NAME}": ===================)
project(${TARGET_NAME} VERSION 0.0.0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SRC_FILES *.h ../../libs/glad/src/glad.c)
set(PUBLIBS "C:/test/opengl_study")
#PATHS "E:/opengl/publibs/libglfw/lib/cmake/glfw3"
# find_package(GLFW glfw3 PATHS "E:/opengl/publibs/libglfw"  REQUIRED)
include_directories(${PUBLIBS}/include)
link_directories(${PUBLIBS}/libs)
include_directories(../../libs/glad/include)

# add_executable(${PROJECT_NAME} ${SRC_FILES} hello_triangle.cpp)
# add_executable(${PROJECT_NAME} ${SRC_FILES} hello_triangle_juxing.cpp)
# add_executable(${PROJECT_NAME} ${SRC_FILES} hello_triangle_indexs.cpp)
# add_executable(${PROJECT_NAME} ${SRC_FILES} yuv-player.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES} openl_yuv.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glfw3dll)


SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})


