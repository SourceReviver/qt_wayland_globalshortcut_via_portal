#pragma once
#include <QObject>
#include <QtDBus/QtDBus>

// a(sa{sv})
using Shortcuts = QList<QPair<QString, QVariantMap>>;

Q_DECLARE_METATYPE(Shortcuts)

class QWindow;

class wayland_shortcut : public QObject
{
  Q_OBJECT

public:
  wayland_shortcut(QWindow* w);

  void list_shortcuts();

public Q_SLOTS:
  void process_create_session_response(uint, const QVariantMap& results);
  void process_list_shortcuts_response(uint, const QVariantMap& results);
  void process_activated_signal(const QDBusObjectPath& session_handle,
                                const QString& shortcut_id,
                                qulonglong timestamp,
                                const QVariantMap& options);

private:
  const QString handle_token = "This_is_handle_token_name";
  const QString session_handle_token = "This_is_session_handle_name";

  QDBusObjectPath response_handle;
  QDBusObjectPath session_obj_path;
  QString protol_window_handler_id;
  QString obtain_protol_window_handler(QWindow*);
};
