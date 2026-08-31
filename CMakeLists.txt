cmake_minimum_required(VERSION 3.19)
project(secureboot-manager LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Declare the proper matching executable binary name target
qt_add_executable(secureboot-manager
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mokkeyentry.h
    mokuiadapter.h
    mokuiadapter.cpp
    mokmockprovider.h
    mokmockprovider.cpp
    mokselectiontracker.h
    mokselectiontracker.cpp
)


target_link_libraries(secureboot-manager
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

# Install location rules matching standard Debian setups
install(TARGETS secureboot-manager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
