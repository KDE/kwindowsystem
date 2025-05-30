add_library(KF6WindowSystemKWaylandPlugin MODULE)
set(wayland_plugin_SRCS
    plugin.cpp
    shm.cpp
    windoweffects.cpp
    windowshadow.cpp
    windowsystem.cpp
    waylandxdgactivationv1.cpp
    waylandxdgdialogv1.cpp
    waylandxdgforeignv2.cpp
    plugin.h
    windoweffects.h
    windowshadow.h
    windowsystem.h
    waylandxdgactivationv1_p.h
)


if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
    set(private_code_option  "PRIVATE_CODE")
endif()
qt6_generate_wayland_protocol_client_sources(KF6WindowSystemKWaylandPlugin
    ${private_code_option}
    FILES
        ${WaylandProtocols_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
        ${WaylandProtocols_DATADIR}/unstable/xdg-foreign/xdg-foreign-unstable-v2.xml
        ${WaylandProtocols_DATADIR}/staging/xdg-dialog/xdg-dialog-v1.xml
        ${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/blur.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/contrast.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/slide.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/plasma-window-management.xml
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/shadow.xml
        ${Wayland_DATADIR}/wayland.xml
)


ecm_qt_declare_logging_category(wayland_plugin_SRCS
    HEADER logging.h
    IDENTIFIER KWAYLAND_KWS
    CATEGORY_NAME kf.windowsystem.wayland
    OLD_CATEGORY_NAMES org.kde.kf5.kwindowsystem.kwayland
    DEFAULT_SEVERITY Warning
    DESCRIPTION "KWindowSystem Wayland Plugin"
    EXPORT KWINDOWSYSTEM
)

target_sources(KF6WindowSystemKWaylandPlugin PRIVATE ${wayland_plugin_SRCS})

if(HAVE_MEMFD)
    target_compile_definitions(KF6WindowSystemKWaylandPlugin PRIVATE -DHAVE_MEMFD)
endif()


target_compile_options(KF6WindowSystemKWaylandPlugin PRIVATE -UQT_NO_KEYWORDS)

pkg_check_modules(XKBCommon REQUIRED IMPORTED_TARGET xkbcommon)

target_link_libraries(KF6WindowSystemKWaylandPlugin
    KF6WindowSystem
    Wayland::Client
    Qt::GuiPrivate
    Qt::WaylandClient
    PkgConfig::XKBCommon
)


install(
    TARGETS
        KF6WindowSystemKWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf6/kwindowsystem/
)
