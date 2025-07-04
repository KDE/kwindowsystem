cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.17.0") # handled by release scripts
project(KWindowSystem VERSION ${KF_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 6.16.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakeFindFrameworks)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMPoQmTools)
include(ECMGeneratePkgConfigFile)
include(ECMQmlModule)
include(ECMGenerateQDoc)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

set(kwindowsystem_version_header "${CMAKE_CURRENT_BINARY_DIR}/src/kwindowsystem_version.h")
ecm_setup_version(PROJECT VARIABLE_PREFIX KWINDOWSYSTEM
                        VERSION_HEADER "${kwindowsystem_version_header}"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF6WindowSystemConfigVersion.cmake"
                        SOVERSION 6)

# Dependencies
set(REQUIRED_QT_VERSION 6.7.0)
find_package(Qt6Gui ${REQUIRED_QT_VERSION} CONFIG REQUIRED)

if (Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
    find_package(Qt6GuiPrivate ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
endif()

option(KWINDOWSYSTEM_QML "Build QML bindings" ON)

if (KWINDOWSYSTEM_QML)
    find_package(Qt6Qml ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
endif()

option(KWINDOWSYSTEM_X11 "Build X11 support" ON)
option(KWINDOWSYSTEM_WAYLAND "Build Wayland support" ON)

if (WIN32 OR APPLE OR ANDROID OR HAIKU)
    set(KWINDOWSYSTEM_X11 OFF)
    set(KWINDOWSYSTEM_WAYLAND OFF)
endif()

if (KWINDOWSYSTEM_X11)
    find_package(X11 REQUIRED)
    find_package(XCB COMPONENTS REQUIRED XCB KEYSYMS RES ICCCM)
endif()

if (KWINDOWSYSTEM_WAYLAND)
    find_package(Qt6WaylandClient ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
    find_package(WaylandProtocols 1.21 REQUIRED)
    find_package(PlasmaWaylandProtocols REQUIRED)
    check_cxx_source_compiles("
        #include <fcntl.h>
        #include <sys/mman.h>
        #include <unistd.h>

        int main() {
            const int size = 10;
            int fd = memfd_create(\"test\", MFD_CLOEXEC | MFD_ALLOW_SEALING);
            ftruncate(fd, size);
            fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK | F_SEAL_SEAL);
            mmap(nullptr, size, PROT_WRITE, MAP_SHARED, fd, 0);
            return 0;
        }" HAVE_MEMFD)
endif()

set(KWINDOWSYSTEM_HAVE_X11 ${KWINDOWSYSTEM_X11})

# Subdirectories
ecm_install_po_files_as_qm(poqm)

ecm_set_disabled_deprecation_versions(
    QT 6.9.0
)

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF6WindowSystem")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF6WindowSystemConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF6WindowSystemConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF6WindowSystemConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF6WindowSystemConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF6WindowSystemTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF6WindowSystemTargets.cmake NAMESPACE KF6:: )


install(FILES ${kwindowsystem_version_header}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KWindowSystem COMPONENT Devel )

if (NOT WIN32)
    ecm_generate_pkgconfig_file(BASE_NAME KF6WindowSystem
      INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KWindowSystem
      DEPS Qt6Gui
    INSTALL)
endif()

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
