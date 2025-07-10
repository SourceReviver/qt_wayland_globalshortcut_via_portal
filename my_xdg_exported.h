#pragma once

#include "qwayland-xdg-foreign-unstable-v2.h"
#include <QWaylandClientExtension>
#include <QWindow>
#include <QGuiApplication>

/// These code are to obtain correct window handle
/// As of 2025 July, it is irrelevant
/// https://flatpak.github.io/xdg-desktop-portal/docs/window-identifiers.html

class MyXdgExporter;
class my_xdg_exported : public QObject, public QtWayland::zxdg_exported_v2
{
public:
  explicit my_xdg_exported(MyXdgExporter* my_xdg_exporter);
  void zxdg_exported_v2_handle(const QString& handle) override;
  QString exported_window_handle();
  void setWindow(QWindow* window);
  QString m_handle;
  MyXdgExporter * m_exporter;
};

class MyXdgExporter : public QWaylandClientExtensionTemplate<MyXdgExporter>, public QtWayland::zxdg_exporter_v2
{
public:
  MyXdgExporter()
    : QWaylandClientExtensionTemplate(ZXDG_EXPORTER_V2_DESTROY_SINCE_VERSION)
  {
    initialize();
    if (!isInitialized()) {
      qDebug()<<"failed to init my xdg exporter";
    }
    //Q_ASSERT(isInitialized());
  };

  ~MyXdgExporter()
  {
    destroy();
  };

  my_xdg_exported *exportWindow(QWindow *window)
  {
    if (!window) {
      qDebug() << "no window!";
      return nullptr;
    }

    auto exported = new my_xdg_exported(this);
    exported->setWindow(window);
    return exported;
  };
};
