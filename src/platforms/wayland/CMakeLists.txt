add_library(KF${QT_MAJOR_VERSION}WindowSystemKWaylandPlugin MODULE)
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
    ecm_add_qtwayland_client_protocol(wayland_plugin_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/blur.xml
        BASENAME blur
    )
    ecm_add_qtwayland_client_protocol(wayland_plugin_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/contrast.xml
        BASENAME contrast
    )
    ecm_add_qtwayland_client_protocol(wayland_plugin_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/slide.xml
        BASENAME slide
    )
    ecm_add_qtwayland_client_protocol(wayland_plugin_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/plasma-window-management.xml
        BASENAME plasma-window-management
    )
else()
    qt6_generate_wayland_protocol_client_sources(KF${QT_MAJOR_VERSION}WindowSystemKWaylandPlugin FILES
        ${WaylandProtocols_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/blur.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/contrast.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/slide.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/plasma-window-management.xml
    )

endif()
ecm_qt_declare_logging_category(wayland_plugin_SRCS HEADER logging.h IDENTIFIER KWAYLAND_KWS
    CATEGORY_NAME org.kde.kf5.kwindowsystem.kwayland DEFAULT_SEVERITY Warning
    DESCRIPTION "wayland integration (windowsystem)" EXPORT KWAYLAND)

target_sources(KF${QT_MAJOR_VERSION}WindowSystemKWaylandPlugin PRIVATE ${wayland_plugin_SRCS})

pkg_check_modules(XKBCommon REQUIRED IMPORTED_TARGET xkbcommon)

target_link_libraries(KF${QT_MAJOR_VERSION}WindowSystemKWaylandPlugin
    KF${QT_MAJOR_VERSION}::WindowSystem
    KF${QT_MAJOR_VERSION}::WaylandClient
    Qt::Widgets
    Wayland::Client
    Qt::WaylandClient
    Qt::WaylandClientPrivate
    PkgConfig::XKBCommon
)

if (QT_MAJOR_VERSION EQUAL "5")
    target_link_libraries(KF${QT_MAJOR_VERSION}WindowSystemKWaylandPlugin
        Qt::XkbCommonSupportPrivate
    )
endif()

install(
    TARGETS
        KF${QT_MAJOR_VERSION}WindowSystemKWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kwindowsystem/
)

ecm_qt_install_logging_categories(
        EXPORT KWAYLAND
        FILE kwindowsystem.kwayland.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

