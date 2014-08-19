

configure_file(config-kwindowsystem.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kwindowsystem.h )

ecm_create_qm_loader(kwindowsystem_QM_LOADER kwindowsystem5_qt)

set(kwindowsystem_SRCS
    kkeyserver.cpp
    kstartupinfo.cpp
    kusertimestamp.cpp
    kwindoweffects.cpp
    kwindoweffects_dummy.cpp
    kwindowinfo.cpp
    kwindowsystem.cpp
    ${kwindowsystem_QM_LOADER}
    )

if (KWINDOWSYSTEM_HAVE_X11)
  set(kwindowsystem_SRCS ${kwindowsystem_SRCS}
    kselectionowner.cpp
    kselectionwatcher.cpp
    kxerrorhandler.cpp
    kxutils.cpp
  )
endif()

set(platformLinkLibraries)

if (KWINDOWSYSTEM_HAVE_X11)
   if(NOT X11_Xrender_LIB)
      message(FATAL_ERROR "The XRender library could not be found. Please install the development package for it.")
   endif()
   if(NOT X11_Xfixes_LIB)
      message(FATAL_ERROR "The XFixes library could not be found. Please install the development package for it.")
   endif()
   set(platformLinkLibraries Qt5::X11Extras ${X11_LIBRARIES} ${X11_Xfixes_LIB} ${X11_Xrender_LIB} ${XCB_XCB_LIBRARY} ${XCB_KEYSYMS_LIBRARY})
   set(kwindowsystem_SRCS ${kwindowsystem_SRCS} kkeyserver_x11.cpp
                                                kwindowsystem_x11.cpp
                                                kwindowinfo_x11.cpp
                                                kwindoweffects_x11.cpp
                                                kxmessages.cpp
                                                netwm.cpp )
endif ()

if (APPLE)
   set(kwindowsystem_SRCS ${kwindowsystem_SRCS} kkeyserver_mac.cpp
#                                                 kwindowsystem_mac.cpp
# FIXME: adjust kwindowinfo_mac to inherit from KWindowInfoPrivate
#                                                 kwindowinfo_mac.cpp
)
  set(platformLinkLibraries "-framework CoreFoundation -framework Carbon")
endif ()

if (WIN32)
   set(kwindowsystem_SRCS ${kwindowsystem_SRCS} kkeyserver_win.cpp
#                                                 kwindowsystem_win.cpp
# FIXME: adjust kwindowinfo_win to inherit from KWindowInfoPrivate
#                                                 kwindowinfo_win.cpp
)
   set(platformLinkLibraries
      Qt5::WinExtras # QtWin::fromHICON(), QtWin::toHICON()
      Ws2_32 # gethostname()
   )
endif ()

add_library(KF5WindowSystem ${kwindowsystem_SRCS})
generate_export_header(KF5WindowSystem BASE_NAME KWindowSystem)
add_library(KF5::WindowSystem ALIAS KF5WindowSystem)

target_include_directories(KF5WindowSystem INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KWindowSystem>")

target_link_libraries(KF5WindowSystem
    PUBLIC Qt5::Widgets
    PRIVATE ${platformLinkLibraries}
)

if(IS_ABSOLUTE "${KF5_INCLUDE_INSTALL_DIR}")
  target_include_directories(KF5WindowSystem INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}>" )
else()
  target_include_directories(KF5WindowSystem INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${KF5_INCLUDE_INSTALL_DIR}>" )
endif()

set_target_properties(KF5WindowSystem PROPERTIES VERSION   ${KWINDOWSYSTEM_VERSION_STRING}
                                                 SOVERSION ${KWINDOWSYSTEM_SOVERSION}
                                                 EXPORT_NAME WindowSystem
)

ecm_generate_headers(KWindowSystem_HEADERS
  HEADER_NAMES
  KKeyServer
  KSelectionOwner
  KSelectionWatcher
  KStartupInfo
  KUserTimestamp
  KWindowEffects
  KWindowInfo
  KWindowSystem
  KXMessages
  NETWM # does not match the classnames in that file...

  REQUIRED_HEADERS KWindowSystem_HEADERS
)

install(TARGETS KF5WindowSystem EXPORT KF5WindowSystemTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  # FIXME: It seems odd to install this.
  ${CMAKE_CURRENT_BINARY_DIR}/config-kwindowsystem.h
  ${CMAKE_CURRENT_BINARY_DIR}/kwindowsystem_export.h
  ${KWindowSystem_HEADERS}
  netwm_def.h
  kmanagerselection.h
  fixx11h.h
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KWindowSystem COMPONENT Devel
)

if (APPLE)
   install( FILES kkeyserver_mac.h DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KWindowSystem COMPONENT Devel)
elseif (WIN32)
    install( FILES kkeyserver_win.h DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KWindowSystem COMPONENT Devel )
else()
   install( FILES kkeyserver_x11.h DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KWindowSystem COMPONENT Devel)
endif ()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KWindowSystem LIB_NAME KF5WindowSystem DEPS "gui" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KF5_INCLUDE_INSTALL_DIR}/KWindowSystem)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
