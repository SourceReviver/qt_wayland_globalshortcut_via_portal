#include <QApplication>
#include <QPushButton>
#include <QWindow>
#include <QtDBus/QtDBus>

#include "wayland_shortcut.h"

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QApplication::setApplicationName("TheAppName");
  QApplication::setOrganizationDomain("org.nice");

  auto* btn = new QPushButton("useless button");
  btn->show();
  btn->windowHandle()->setTitle("useless title");

  auto* shortcut = new wayland_shortcut(btn->windowHandle());

  return QApplication::exec();
}
