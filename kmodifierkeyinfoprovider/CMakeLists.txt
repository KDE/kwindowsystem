add_library(kmodifierkey_wayland MODULE kmodifierkeyinfoprovider_wayland.cpp)
target_link_libraries(kmodifierkey_wayland PRIVATE KF5::WaylandClient KF5::GuiAddons)
install(TARGETS kmodifierkey_wayland DESTINATION ${PLUGIN_INSTALL_DIR}/kf5/kguiaddons/kmodifierkey/)
