#include "my_xdg_exported.h"
#include <qpa/qplatformnativeinterface.h>

my_xdg_exported::my_xdg_exported(MyXdgExporter* my_xdg_exporter)
  : QtWayland::zxdg_exported_v2()
  , m_exporter(my_xdg_exporter)
{
}
void
my_xdg_exported::zxdg_exported_v2_handle(const QString& handle)
{
  qDebug() << "Maigc handle somehow arrived.";
  qDebug()<< handle;
  m_handle = handle;
}

QString
my_xdg_exported::exported_window_handle()
{
  QString ret = QString("wayland:%1").arg(this->m_handle);
  qDebug() << "handle is -> " << ret;
  return ret;
}


void
my_xdg_exported::setWindow(QWindow* window)
{
  QPlatformNativeInterface *nativeInterface = qGuiApp->platformNativeInterface();
  auto surface = static_cast<wl_surface *>(nativeInterface->nativeResourceForWindow("surface", window));
  if (surface) {
    auto tl = m_exporter->export_toplevel(surface);
    if (tl) {
      init(tl);
    } else {
      qDebug() << "could not export top level";
    }
  } else {
    qDebug() << "could not get surface";
  }
}