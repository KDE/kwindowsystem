set(wayland_plugin_SRCS
    logging.cpp
    plugin.cpp
    waylandintegration.cpp
    windoweffects.cpp
    windowinfo.cpp
    windowshadow.cpp
    windowsystem.cpp
    waylandxdgactivationv1.cpp
)

ecm_add_qtwayland_client_protocol(wayland_plugin_SRCS
    PROTOCOL ${WaylandProtocols_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
    BASENAME xdg-activation-v1
)
ecm_qt_declare_logging_category(wayland_plugin_SRCS HEADER logging.h IDENTIFIER KWAYLAND_KWS
    CATEGORY_NAME org.kde.kf5.kwindowsystem.kwayland DEFAULT_SEVERITY Warning
    DESCRIPTION "wayland integration (windowsystem)" EXPORT KWAYLAND)

pkg_check_modules(XKBCommon REQUIRED IMPORTED_TARGET xkbcommon)

add_library(KF5WindowSystemKWaylandPlugin MODULE ${wayland_plugin_SRCS})
target_link_libraries(KF5WindowSystemKWaylandPlugin
    KF5::WindowSystem
    KF5::WaylandClient
    Qt::Widgets
    Wayland::Client
    Qt::WaylandClient
    Qt::WaylandClientPrivate
    Qt::XkbCommonSupportPrivate
    PkgConfig::XKBCommon
)

install(
    TARGETS
        KF5WindowSystemKWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf5/kwindowsystem/
)

ecm_qt_install_logging_categories(
        EXPORT KWAYLAND
        FILE kwindowsystem.kwayland.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

