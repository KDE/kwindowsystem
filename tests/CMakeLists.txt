remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_TO_ASCII)

include(ECMMarkAsTest)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Widgets)

macro(kwindowsystem_executable_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    target_link_libraries(${_testname} Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Widgets KF5::WindowSystem)
    ecm_mark_as_test(${_testname})
  endforeach(_testname)
endmacro()

kwindowsystem_executable_tests(
  setmainwindowtest
  createpixmapfromhandletest
  blurbehindtest
  backgroundcontrasttest
)

if(KWINDOWSYSTEM_HAVE_X11)
    kwindowsystem_executable_tests(icontest)
    if (TARGET Qt5::X11Extras)
        target_link_libraries(icontest Qt5::X11Extras)
    elseif (TARGET Qt6::Gui)
        target_link_libraries(icontest Qt6::GuiPrivate)
    endif()
endif()
