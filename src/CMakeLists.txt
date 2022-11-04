configure_file(config-kwindowsystem.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kwindowsystem.h )

add_library(KF5WindowSystem)
add_library(KF5::WindowSystem ALIAS KF5WindowSystem)

ecm_create_qm_loader(KF5WindowSystem kwindowsystem5_qt)

target_sources(KF5WindowSystem PRIVATE
    kkeyserver.cpp
    kstartupinfo.cpp
    kusertimestamp.cpp
    kwindoweffects.cpp
    kwindoweffects_dummy.cpp
    kwindowinfo.cpp
    kwindowshadow.cpp
    kwindowsystem.cpp
    pluginwrapper.cpp
    kwindowsystemplugininterface.cpp
    )
ecm_qt_declare_logging_category(KF5WindowSystem
    HEADER kwindowsystem_debug.h
    IDENTIFIER LOG_KWINDOWSYSTEM
    CATEGORY_NAME kf.windowsystem
    OLD_CATEGORY_NAMES org.kde.kwindowsystem
    DEFAULT_SEVERITY Warning
    DESCRIPTION "KWindowSystem"
    EXPORT KWINDOWSYSTEM
)
ecm_qt_declare_logging_category(KF5WindowSystem
    HEADER kwindowsystem_xcb_debug.h
    IDENTIFIER LOG_KKEYSERVER_X11
    CATEGORY_NAME kf.windowsystem.keyserver.x11
    OLD_CATEGORY_NAMES org.kde.kwindowsystem.keyserver.x11
    DEFAULT_SEVERITY Warning
    DESCRIPTION "KKeyServer (KWindowSystem)"
    EXPORT KWINDOWSYSTEM
)

if (KWINDOWSYSTEM_HAVE_X11)
  target_sources(KF5WindowSystem PRIVATE
    platforms/xcb/kselectionowner.cpp
    platforms/xcb/kselectionwatcher.cpp
    platforms/xcb/kxerrorhandler.cpp
    platforms/xcb/kxutils.cpp
    kx11extras.cpp
  )
endif()

if (KWINDOWSYSTEM_HAVE_X11)
   if(NOT X11_Xfixes_LIB)
      message(FATAL_ERROR "The XFixes library could not be found. Please install the development package for it.")
   endif()
   target_include_directories(KF5WindowSystem
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/platforms/xcb
   )

    if (TARGET Qt5::X11Extras)
        set(_qt_x11_libs Qt5::X11Extras)
    elseif (TARGET Qt6::Gui)
        set(_qt_x11_libs Qt6::GuiPrivate) # qtx11extras_p.h
    endif()


   target_link_libraries(KF5WindowSystem
        PUBLIC
            # public because they are used in kkeyserver_x11.h
            X11::X11
        PRIVATE
            XCB::XCB
            X11::Xfixes
            XCB::KEYSYMS
            ${_qt_x11_libs}
   )

   target_sources(KF5WindowSystem PRIVATE
        platforms/xcb/kkeyserver.cpp
        platforms/xcb/kxmessages.cpp
        platforms/xcb/netwm.cpp
   )

   # we install kkeyserver_x11.h which needs the X11 headers available
   # if we don't add the include path here code that includes kkeyserver.h will fail
   # to compile unless X11 is installed in /usr/include
   target_include_directories(KF5WindowSystem PUBLIC ${XCB_XCB_INCLUDE_DIR})
endif ()

if (APPLE)
   target_include_directories(KF5WindowSystem
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/platforms/osx)
   target_sources(KF5WindowSystem PRIVATE
        platforms/osx/kkeyserver.cpp
#                                                 kwindowsystem_mac.cpp
# FIXME: adjust kwindowinfo_mac to inherit from KWindowInfoPrivate
#                                                 kwindowinfo_mac.cpp
   )
   target_link_libraries(KF5WindowSystem
        PRIVATE
            "-framework CoreFoundation -framework Carbon"
   )
endif ()

if (WIN32)
   target_include_directories(KF5WindowSystem
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/platforms/windows
   )
   target_sources(KF5WindowSystem PRIVATE
        platforms/windows/kkeyserver.cpp
#                                                 kwindowsystem_win.cpp
# FIXME: adjust kwindowinfo_win to inherit from KWindowInfoPrivate
#                                                 kwindowinfo_win.cpp
   )
   target_link_libraries(KF5WindowSystem PRIVATE ws2_32) # gethostname()
   if (QT_MAJOR_VERSION EQUAL "5")
      target_link_libraries(KF5WindowSystem PRIVATE Qt5::WinExtras) # QtWin::fromHICON(), QtWin::toHICON()
   endif()
endif ()

ecm_generate_export_header(KF5WindowSystem
    BASE_NAME KWindowSystem
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.0 5.18 5.38 5.62 5.67 5.69 5.80 5.81 5.82 5.101
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5WindowSystem INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KWindowSystem>")

target_link_libraries(KF5WindowSystem
    PUBLIC Qt${QT_MAJOR_VERSION}::Gui
)
if(NOT KWINDOWSYSTEM_NO_WIDGETS)
    target_link_libraries(KF5WindowSystem PUBLIC Qt${QT_MAJOR_VERSION}::Widgets)
endif()

set_target_properties(KF5WindowSystem PROPERTIES VERSION   ${KWINDOWSYSTEM_VERSION}
                                                 SOVERSION ${KWINDOWSYSTEM_SOVERSION}
                                                 EXPORT_NAME WindowSystem
)

ecm_generate_headers(KWindowSystem_HEADERS
  HEADER_NAMES
  KKeyServer
  KStartupInfo
  KUserTimestamp
  KWindowEffects
  KWindowInfo
  KWindowShadow,KWindowShadowTile
  KWindowSystem

  REQUIRED_HEADERS KWindowSystem_HEADERS
)

if (KWINDOWSYSTEM_HAVE_X11)
    ecm_generate_headers(KWindowSystem_HEADERS
    HEADER_NAMES
    KX11Extras

    REQUIRED_HEADERS KWindowSystem_HEADERS
    )
endif()

install(TARGETS KF5WindowSystem EXPORT KF5WindowSystemTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  # FIXME: It seems odd to install this.
  ${CMAKE_CURRENT_BINARY_DIR}/config-kwindowsystem.h
  ${CMAKE_CURRENT_BINARY_DIR}/kwindowsystem_export.h
  ${KWindowSystem_HEADERS}
  netwm_def.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KWindowSystem COMPONENT Devel
)

install(
    FILES
        kwindoweffects_p.h
        kwindowinfo_p.h
        kwindowshadow_p.h
        kwindowsystem_p.h
        kwindowsystemplugininterface_p.h
    DESTINATION
        ${KDE_INSTALL_INCLUDEDIR_KF}/KWindowSystem/private
    COMPONENT
        Devel
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5WindowSystem_QCH
        NAME KWindowSystem
        BASE_NAME KF5WindowSystem
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCE_DIRS
            # using dir for now, to cover any platform dependent code
            # TODO: should only use public headers, to cover only public API
            ${CMAKE_CURRENT_SOURCE_DIR}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Gui_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KWINDOWSYSTEM_EXPORT
            KWINDOWSYSTEM_DEPRECATED
            KWINDOWSYSTEM_DEPRECATED_EXPORT
            "KWINDOWSYSTEM_DEPRECATED_VERSION(x, y, t)"
            "KWINDOWSYSTEM_DEPRECATED_VERSION_BELATED(x, y, xt, yt, t)"
            "KWINDOWSYSTEM_ENUMERATOR_DEPRECATED_VERSION(x, y, t)"
            "KWINDOWSYSTEM_ENUMERATOR_DEPRECATED_VERSION_BELATED(x, y, xt, yt, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

add_subdirectory(platforms)

ecm_qt_install_logging_categories(
    EXPORT KWINDOWSYSTEM
    FILE kwindowsystem.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KWindowSystem LIB_NAME KF5WindowSystem DEPS "gui" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KWindowSystem)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
