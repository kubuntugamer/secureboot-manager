cmake_minimum_required(VERSION 3.16)
project(secureboot-manager LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# 🏗️ REGISTER ALL FILE TARGETS TO CLEAR THE UNDEFINED LINKER ERRORS
qt_add_executable(secureboot-manager
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mokkeyentry.h
    mokuiadapter.h
    mokuiadapter.cpp
    mokprovider.h
    mokprovider.cpp
    mokgenerator.h
    mokgenerator.cpp
    mokuigenerator.h       # 📥 RE-ADD THE UI GENERATOR HEADER
    mokuigenerator.cpp     # 📥 RE-ADD THE UI GENERATOR SOURCE CODE
    mokselectiontracker.h
    mokselectiontracker.cpp
)

target_link_libraries(secureboot-manager
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

# Install rules matching standard Ubuntu/Debian paths
install(TARGETS secureboot-manager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
