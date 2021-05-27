add_library(KF5WindowSystemWaylandPlugin MODULE)

target_sources(KF5WindowSystemWaylandPlugin PRIVATE
    kwindowsystem.cpp
    plugin.cpp
)

target_link_libraries(KF5WindowSystemWaylandPlugin
    KF5WindowSystem
)

set_target_properties(
    KF5WindowSystemWaylandPlugin
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/kf5/kwindowsystem"
)

install(
    TARGETS
        KF5WindowSystemWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf5/kwindowsystem/
)
