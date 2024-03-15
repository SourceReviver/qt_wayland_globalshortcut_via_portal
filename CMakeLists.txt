cmake_minimum_required(VERSION 3.28)
project(wayland_global_shortcut)

set(CMAKE_CXX_STANDARD 23)

option(USE_DBUS_XML_GEN "typed QtDebus" OFF)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        DBus
        REQUIRED)

set(THE_SRC
        main.cpp
        wayland_shortcut.cpp
        wayland_shortcut.h
)

qt_standard_project_setup()

if (USE_DBUS_XML_GEN)
    set(DBUS_XML_PATH "/usr/share/dbus-1/interfaces")

    qt_add_dbus_interface(THE_SRC "${DBUS_XML_PATH}/org.freedesktop.portal.GlobalShortcuts.xml" globalshortcuts_interface)
    qt_add_dbus_interface(THE_SRC "${DBUS_XML_PATH}/org.freedesktop.portal.Request.xml" portalsrequest_interface)
endif ()

add_executable(wayland_global_shortcut ${THE_SRC})
target_link_libraries(wayland_global_shortcut
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::DBus
        Qt::CorePrivate
        Qt::WidgetsPrivate
)

target_include_directories(wayland_global_shortcut PRIVATE ${CMAKE_BINARY_DIR})
