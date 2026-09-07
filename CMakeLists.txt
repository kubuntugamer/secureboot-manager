cmake_minimum_required(VERSION 3.16)
project(secureboot-manager LANGUAGES CXX)

# Enforce strict Modern C++ configuration properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Discover foundational framework dependencies
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets)

# Enable automatic Meta-Object Compiler (MOC) and UI compilation lanes
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt_standard_project_setup()

# Compile target with verified explicit file path matching
qt_add_executable(secureboot-manager
    # Root Entry and Asset Layer Configurations
    main.cpp
    resources.qrc

    # Presentation & UI Modules (.ui and .cpp files inside ui/)
    ui/mainwindow.cpp
    ui/mainwindow.ui
    ui/mokledgerpage.cpp
    ui/mokaboutdialog.cpp
    src/mokgeneratorpage.cpp
    src/moksignerpage.cpp

    # Mapped core headers out of your actual 'include/' directory block
    # Satisfies CMake's source tree validation and hooks headers using Q_OBJECT into AUTOMOC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokledgerpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokaboutdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokgeneratorpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/moksignerpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokremovalwizard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/debiandiscoveryengine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokautomationbackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokdashboardconsole.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/abstractdiscoveryengine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/moksigningwizard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokenrollmentwizard.h

    # FIX: Added the missing generator header to clear the final vtable linking error!
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokgenerator.h

    # Core Systems Logic Engine
    src/mokautomationbackend.cpp
    src/mokuiadapter.cpp
    src/mokprovider.cpp
    src/mokgenerator.cpp
    src/mokuigenerator.cpp
    src/mokuisigner.cpp
    src/mokselectiontracker.cpp

    # Diagnostics & Monitoring System Console
    src/mokdashboardconsole.cpp

    # Extensible Kernel Discovery Engine
    src/debiandiscoveryengine.cpp

    # Security Interlocks & Defensive Overlay Blocks
    src/mokenrollmentwizard.cpp
    src/mokhijackoverlay.cpp

    # Automated Remediation & MOK Removal Wizard Module Elements
    src/mokremovalwizard.cpp
    src/mokremovalwizard_init.cpp
    src/mokremovalwizard_ledger.cpp
    src/mokremovalwizard_verify.cpp
    src/mokremovalwizard_scanner.cpp
    src/mokremovalwizard_executor.cpp

    # Guided Kernel Signing Wizard Module Elements
    src/moksigningwizard.cpp
)

# Map include directories privately to the target executable architecture
target_include_directories(secureboot-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

# Dynamic Linker binding mappings
target_link_libraries(secureboot-manager
    PRIVATE
        Qt::Core
        Qt::Widgets
)

# Enforce uniform installation path layouts matching Freedesktop specs
include(GNUInstallDirs)

install(TARGETS secureboot-manager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
