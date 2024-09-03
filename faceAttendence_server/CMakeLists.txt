cmake_minimum_required(VERSION 3.16)

project(faceAttendence_server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#opencv package
include_directories(E:/qt_workspace/libs/opencv_452/include/opencv2)
include_directories(E:/qt_workspace/libs/opencv_452/include)

set(OpenCV_DIR "E:/qt_workspace/libs/opencv_452/x64/mingw/lib")
find_package(OpenCV REQUIRED)

#seetaface package
include_directories(E:/qt_workspace/libs/SeetaFace2/include/seeta)
include_directories(E:/qt_workspace/libs/SeetaFace2/include)

set(SeetaFace_DIR "E:/qt_workspace/libs/SeetaFace2/lib/cmake")
find_package(SeetaFace REQUIRED)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql Multimedia MultimediaWidgets)

add_subdirectory(lib/ela)

include_directories(lib/ela/include)

add_subdirectory(lib/QXlsx)

include_directories(lib/QXlsx/header)

include_directories(subPanels)
include_directories(objs)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(faceAttendence_server
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        subPanels/showheadimgpanel.h subPanels/showheadimgpanel.cpp
        objs/faceinfo.h objs/faceinfo.cpp
        dbPathDefines.h
        subPanels/registerpanel.h subPanels/registerpanel.cpp
        imgs.qrc
        subPanels/dbquerypanel.h subPanels/dbquerypanel.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET faceAttendence_server APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

#遍历所有资源文件
file(GLOB_RECURSE RES_PATHS *.png *.jpg *.svg *.ico *.ttf *.webp *.js)
foreach(filepath ${RES_PATHS})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
        list(APPEND resource_files ${filename})
endforeach(filepath)

qt_add_resources(${PROJECT_NAME} "faceAttendence_server"
    RESOURCES PREFIX "/"
    FILES
    ${resource_files}
)


else()
    if(ANDROID)
        add_library(faceAttendence_server SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(faceAttendence_server
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(faceAttendence_server PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    elawidgettools ${OpenCV_LIBS} ${SeetaFace_LIBRARIES}
    QXlsx::QXlsx)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.faceAttendence_server)
endif()
set_target_properties(faceAttendence_server PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS faceAttendence_server
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(faceAttendence_server)
endif()
