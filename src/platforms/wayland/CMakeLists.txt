add_library(KF5WindowSystemKWaylandPlugin MODULE)
set(wayland_plugin_SRCS
    plugin.cpp
    waylandintegration.cpp
    windoweffects.cpp
    windowinfo.cpp
    windowshadow.cpp
    windowsystem.cpp
    waylandxdgactivationv1.cpp
    plugin.h
    waylandintegration.h
    windoweffects.h
    windowinfo.h
    windowshadow.h
    windowsystem.h
    waylandxdgactivationv1_p.h
)
if (QT_MAJOR_VERSION EQUAL "5")
    ecm_add_qtwayland_client_protocol(wayland_plugin_SRCS
        PROTOCOL ${WaylandProtocols_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
        BASENAME xdg-activation-v1
    )
else()
    qt6_generate_wayland_protocol_client_sources(KF5WindowSystemKWaylandPlugin FILES
        ${WaylandProtocols_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
    )

endif()
ecm_qt_declare_logging_category(wayland_plugin_SRCS HEADER logging.h IDENTIFIER KWAYLAND_KWS
    CATEGORY_NAME org.kde.kf5.kwindowsystem.kwayland DEFAULT_SEVERITY Warning
    DESCRIPTION "wayland integration (windowsystem)" EXPORT KWAYLAND)

target_sources(KF5WindowSystemKWaylandPlugin PRIVATE ${wayland_plugin_SRCS})

pkg_check_modules(XKBCommon REQUIRED IMPORTED_TARGET xkbcommon)

target_link_libraries(KF5WindowSystemKWaylandPlugin
    KF5::WindowSystem
    KF5::WaylandClient
    Qt::Widgets
    Wayland::Client
    Qt::WaylandClient
    PkgConfig::XKBCommon
)

if (QT_MAJOR_VERSION EQUAL "5")
    target_link_libraries(KF5WindowSystemKWaylandPlugin
        Qt::WaylandClientPrivate
        Qt::XkbCommonSupportPrivate
    )
endif()

install(
    TARGETS
        KF5WindowSystemKWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kwindowsystem/
)

ecm_qt_install_logging_categories(
        EXPORT KWAYLAND
        FILE kwindowsystem.kwayland.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

