add_library(kmodifierkey_wayland MODULE kmodifierkeyinfoprovider_wayland.cpp kmodifierkeyinfoprovider_wayland.h)
target_link_libraries(kmodifierkey_wayland PRIVATE KF5::WaylandClient KF5::GuiAddons)
install(TARGETS kmodifierkey_wayland DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kguiaddons/kmodifierkey/)
