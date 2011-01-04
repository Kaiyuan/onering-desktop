#ifndef APP_H
#define APP_H

#include <QString>
#include <QUrl>

int is_appname_registered(const QString &appname);

QByteArray call_app(const char* method, const QUrl &url, const char* body=NULL);
QByteArray call_app_body(const char* method, const QUrl &url, const char* body=NULL);

#endif
