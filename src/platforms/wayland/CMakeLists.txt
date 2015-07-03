set(wayland_plugin_SRCS
    logging.cpp
    plugin.cpp
    windowsystem.cpp
)

add_library(KF5WindowSystemKWaylandPlugin MODULE ${wayland_plugin_SRCS})
target_link_libraries(KF5WindowSystemKWaylandPlugin
    KF5::WindowSystem
    KF5::WaylandClient
)

install(
    TARGETS
        KF5WindowSystemKWaylandPlugin
    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kf5/org.kde.kwindowsystem.platforms/
)

