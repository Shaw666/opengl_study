cmake_minimum_required(VERSION 3.5)

project(test_opengl VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/xhp/Qt/5.15.2/gcc_64")
set(PUBLIBS "/home/xhp/work/publibs/64" CACHE PATH "")
include_directories(${PUBLIBS}/libffmpeg/include)
link_directories("/home/xhp/test/build-QtAV-Desktop_Qt_5_15_2_GCC_64bit-Debug/lib_linux_x86_64")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets OpenGL REQUIRED)
#find_package(QtAV REQUIRED)

include_directories("/home/xhp/Qt/5.15.2/gcc_64/include")
link_directories(${PUBLIBS}/libffmpeg/lib)

file(GLOB PROJECT_SOURCES *.cpp *.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(test_opengl
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET test_opengl APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(test_opengl SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(test_opengl
            ${PROJECT_SOURCES}
        )
    endif()
endif()

#target_link_libraries(test_opengl PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGL ${QTAVWIDGETS_LIBRARIES})
target_link_libraries(test_opengl PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGL QtAV QtAVWidgets)
set_target_properties(test_opengl PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(test_opengl)
endif()
