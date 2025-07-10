cmake_minimum_required(VERSION 3.28)
project(wayland_global_shortcut)

set(CMAKE_CXX_STANDARD 23)

option(USE_DBUS_XML_GEN "typed QtDebus" OFF)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        DBus
        WaylandClient
        REQUIRED)

qt_standard_project_setup()

add_executable(wayland_global_shortcut
        main.cpp
        wayland_shortcut.cpp
        my_xdg_exported.cpp
)
target_link_libraries(wayland_global_shortcut
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::DBus
        Qt::CorePrivate
        Qt::WidgetsPrivate
        Qt::WaylandClient
)

qt_generate_wayland_protocol_client_sources(
        wayland_global_shortcut
        PUBLIC_CODE
        FILES "/usr/share/wayland-protocols/unstable/xdg-foreign/xdg-foreign-unstable-v2.xml")