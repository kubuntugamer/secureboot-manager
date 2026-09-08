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
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc

    # Presentation & UI Modules (.ui and .cpp files inside ui/)
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mainwindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mokledgerpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/mokaboutdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokgeneratorpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/moksignerpage.cpp

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokenrollmentwizard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mokuisigner.h

    # Modernized Sub-Module Headers (Split)
    ${CMAKE_CURRENT_SOURCE_DIR}/include/keyscanner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/signingengine.h

    # Core Systems Logic Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokautomationbackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuiadapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuigenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokselectiontracker.cpp

    # Modularized UI Signer Modules (Split)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuisigner_init.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokuisigner_executor.cpp

    # Modernized Sub-Module Source Files (Split)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keyscanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/signingengine.cpp

    # Diagnostics & Monitoring System Console
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokdashboardconsole.cpp

    # Extensible Kernel Discovery Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debiandiscoveryengine.cpp

    # Security Interlocks & Defensive Overlay Blocks
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokenrollmentwizard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokhijackoverlay.cpp

    # Automated Remediation & MOK Removal Wizard Module Elements
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokremovalwizard_init.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokremovalwizard_ledger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokremovalwizard_verify.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokremovalwizard_scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mokremovalwizard_executor.cpp
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
