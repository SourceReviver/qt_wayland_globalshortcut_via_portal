#include "wayland_shortcut.h"

#include <private/qgenericunixservices_p.h>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include <QtDBus/QtDBus>

wayland_shortcut::
wayland_shortcut(QWindow* w)
{
  protol_window_handler_id = obtain_protol_window_handler(w);

  QDBusMessage create_session =
    QDBusMessage::createMethodCall("org.freedesktop.portal.Desktop",
                                   "/org/freedesktop/portal/desktop",
                                   "org.freedesktop.portal.GlobalShortcuts",
                                   "CreateSession");

  QList<QVariant> args_create_session;

  QMap<QString, QVariant> options_create_session;
  options_create_session.insert("handle_token", handle_token);
  options_create_session.insert("session_handle_token", session_handle_token);

  args_create_session.append(options_create_session);

  create_session.setArguments(args_create_session);

  QDBusMessage c = QDBusConnection::sessionBus().call(create_session);
  this->response_handle = c.arguments().first().value<QDBusObjectPath>();

  qDebug() << "Return from create session ->" << c;

  QDBusConnection::sessionBus().connect(
    "org.freedesktop.portal.Desktop",
    response_handle.path(),
    QLatin1String("org.freedesktop.portal.Request"),
    QLatin1String("Response"),
    this,
    SLOT(process_create_session_response(uint, QVariantMap)));
}

void
wayland_shortcut::list_shortcuts()
{
  QDBusMessage list_shortcut =
    QDBusMessage::createMethodCall("org.freedesktop.portal.Desktop",
                                   "/org/freedesktop/portal/desktop",
                                   "org.freedesktop.portal.GlobalShortcuts",
                                   "ListShortcuts");

  QList<QVariant> args_list_shotcuts;
  args_list_shotcuts.append(QVariant::fromValue(session_obj_path));
  QMap<QString, QVariant> options_list_shortcut;
  options_list_shortcut.insert("handle_token", handle_token);

  args_list_shotcuts.append(options_list_shortcut);

  list_shortcut.setArguments(args_list_shotcuts);

  QDBusMessage list_ret = QDBusConnection::sessionBus().call(list_shortcut);
  qDebug() << "list_shortcuts message return ->" << list_ret;

  QDBusConnection::sessionBus().connect(
    "org.freedesktop.portal.Desktop",
    list_ret.arguments().first().value<QDBusObjectPath>().path(),
    QLatin1String("org.freedesktop.portal.Request"),
    QLatin1String("Response"),
    this,
    SLOT(process_list_shortcuts_response(uint, QVariantMap)));
}

QString
wayland_shortcut::obtain_protol_window_handler(QWindow* w)
{
  // TODO: Private API could be gone
  if (const auto services = dynamic_cast<QGenericUnixServices*>(
        QGuiApplicationPrivate::platformIntegration()->services())) {
    return services->portalWindowIdentifier(w).toUtf8().constData();
  }
}

void
wayland_shortcut::process_create_session_response(uint i,
                                                  const QVariantMap& results)
{
  qDebug() << "result ->" << results;
  if (results.contains("session_handle")) {
    QString session_handle = results["session_handle"].toString();
    this->session_obj_path = QDBusObjectPath(session_handle);
  };

  QDBusConnection::sessionBus().disconnect(
    "org.freedesktop.portal.Desktop",
    response_handle.path(),
    QLatin1String("org.freedesktop.portal.Request"),
    QLatin1String("Response"),
    this,
    SLOT(process_create_session_response(uint, QVariantMap)));

  QDBusMessage bind_shortcut =
    QDBusMessage::createMethodCall("org.freedesktop.portal.Desktop",
                                   "/org/freedesktop/portal/desktop",
                                   "org.freedesktop.portal.GlobalShortcuts",
                                   "BindShortcuts");

  QList<QVariant> bind_shortcut_args;
  bind_shortcut_args.append(session_obj_path);

  Shortcuts shortcuts;

  qDBusRegisterMetaType<std::pair<QString, QVariantMap>>();
  qDBusRegisterMetaType<Shortcuts>();

  Q_ASSERT(QLatin1String(QDBusMetaType::typeToSignature(QMetaType(
             qMetaTypeId<Shortcuts>()))) == QLatin1String("a(sa{sv})"));

  QPair<QString, QVariantMap> a_shortcut;

  QVariantMap a_shortcut_options;
  a_shortcut.first = "nice";
  a_shortcut_options.insert("description", "do sth");
  a_shortcut_options.insert("preferred_trigger", "CTRL+m");
  a_shortcut.second = a_shortcut_options;

  QPair<QString, QVariantMap> b_shortcut;

  QVariantMap b_shortcut_options;
  b_shortcut.first = "nice 2";
  b_shortcut_options.insert("description", "do sth 2");
  b_shortcut_options.insert("preferred_trigger", "CTRL+n");
  b_shortcut.second = b_shortcut_options;

  shortcuts.append(a_shortcut);
  shortcuts.append(b_shortcut);

  bind_shortcut_args.append(QVariant::fromValue(shortcuts));
  bind_shortcut_args.append(
    protol_window_handler_id); // Wayland window id, could be empty for now??
                               // (useless so far)

  QMap<QString, QVariant> bind_opts;
  bind_opts.insert("handle_token", handle_token);

  bind_shortcut_args.append(bind_opts);

  bind_shortcut.setArguments(bind_shortcut_args);
  qDebug() << "input of bind->" << bind_shortcut.arguments();

  QDBusMessage bind_ret = QDBusConnection::sessionBus().call(bind_shortcut);
  qDebug() << "bind message return->" << bind_ret;

  // process signals

  QDBusConnection::sessionBus().connect(
    "org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.GlobalShortcuts",
    "Activated",
    this,
    SLOT(process_activated_signal(
      QDBusObjectPath, QString, qulonglong, QVariantMap)));

  this->list_shortcuts();
}

void
wayland_shortcut::process_list_shortcuts_response(uint,
                                                  const QVariantMap& results)
{
  const auto arg = results["shortcuts"].value<QDBusArgument>();
  Shortcuts s;
  arg >> s;
  for (auto it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it) {
    qDebug() << "Registered shortcuts: " << it->first
             << it->second["description"].toString()
             << it->second["trigger_description"].toString();
  }
}

void
wayland_shortcut::process_activated_signal(
  const QDBusObjectPath& session_handle,
  const QString& shortcut_id,
  qulonglong timestamp,
  const QVariantMap& options)
{
  qDebug() << "Got Signal ->" << session_handle.path() << shortcut_id
           << timestamp << options;
}
