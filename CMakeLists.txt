cmake_minimum_required(VERSION 3.5)

set(KF_VERSION "5.80.0") # handled by release scripts
project(KWindowSystem VERSION ${KF_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.79.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakeFindFrameworks)
include(ECMQtDeclareLoggingCategory)

include(ECMAddQch)
include(ECMPoQmTools)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

# TODO KF6: remove
option(KWINDOWSYSTEM_NO_WIDGETS "Disable deprecated API including QWidget usage, so that KWindowSystem does not link to QtWidgets. Warning: this is binary and source incompatible.")
if (KWINDOWSYSTEM_NO_WIDGETS)
    # ensure EXCLUDE_DEPRECATED_BEFORE_AND_AT is at least at the version where all QWidgets API is deprecated
    if (NOT EXCLUDE_DEPRECATED_BEFORE_AND_AT STREQUAL "CURRENT" AND
        EXCLUDE_DEPRECATED_BEFORE_AND_AT VERSION_LESS "5.62.0")
        set(EXCLUDE_DEPRECATED_BEFORE_AND_AT "5.62.0")
    endif()
endif()

ecm_setup_version(PROJECT VARIABLE_PREFIX KWINDOWSYSTEM
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kwindowsystem_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5WindowSystemConfigVersion.cmake"
                        SOVERSION 5)

# Dependencies
set(REQUIRED_QT_VERSION 5.14.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui Widgets)

if (APPLE)
    cmake_find_frameworks(Carbon)
    cmake_find_frameworks(CoreFoundation)
endif()

if(WIN32)
   find_package(Qt5WinExtras REQUIRED)
endif()

if (NOT APPLE)
    find_package(X11)
endif()

set(KWINDOWSYSTEM_HAVE_X11 ${X11_FOUND})

if(X11_FOUND)
    find_package(XCB COMPONENTS REQUIRED XCB KEYSYMS RES)
    find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED X11Extras)
    set_package_properties(X11_Xrender PROPERTIES DESCRIPTION "X Rendering Extension (libXrender)"
                           URL "http://www.x.org" TYPE RECOMMENDED
                           PURPOSE "Support for compositing, rendering operations, and alpha-blending")
    set(KWINDOWSYSTEM_HAVE_XRENDER ${X11_Xrender_FOUND})
    set(KWINDOWSYSTEM_HAVE_XFIXES ${X11_Xfixes_FOUND})
endif()

# Subdirectories
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5WindowSystem")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5WindowSystem_QCH
        FILE KF5WindowSystemQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5WindowSystemQchTargets.cmake\")")
endif()

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5WindowSystemConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5WindowSystemConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5WindowSystemConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5WindowSystemConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5WindowSystemTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5WindowSystemTargets.cmake NAMESPACE KF5:: )


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kwindowsystem_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
