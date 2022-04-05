set(idletime_plugin_SRCS
    poller.cpp
    poller.h
)

add_library(KF5IdleTimeKWaylandPlugin MODULE ${idletime_plugin_SRCS})
target_link_libraries(KF5IdleTimeKWaylandPlugin
    KF5::IdleTime
    KF5::WaylandClient
)

install(
    TARGETS
        KF5IdleTimeKWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/org.kde.kidletime.platforms/
)
