cmake_minimum_required(VERSION 3.16)
project(secureboot-manager LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

# 🔍 MAP THE INCLUDE DIRECTORY SO HEADERS ARE FOUND NATIVELY EVERYWHERE
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 🗺️ REGISTER RESTRUCTURED FILE TARGETS WITH EXPLICIT SOURCE DIRECTORY PATHS
qt_add_executable(secureboot-manager
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc

    # 🎨 Presentation & View Department
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mainwindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokledgerpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mokledgerpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokgeneratorpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mokgeneratorpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/moksignerpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/moksignerpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokaboutdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mokaboutdialog.cpp

    # 🛡️ Core Logic & Submodule Department
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokautomationbackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokautomationbackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokkeyentry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokuiadapter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuiadapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokprovider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokuigenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuigenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokuisigner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuisigner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokselectiontracker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokselectiontracker.cpp

    # 📦 New Dedicated Diagnostics Interface Submodule
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokdashboardconsole.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokdashboardconsole.cpp
)

target_link_libraries(secureboot-manager
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS secureboot-manager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
