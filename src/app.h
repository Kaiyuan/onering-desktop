#ifndef APP_H
#define APP_H

#include <QString>

int is_appname_registered(const QString &appname);

QByteArray call_app(const char* appname, const char* method, const char* path, const char* body=NULL);
QByteArray call_app_body(const char* appname, const char* method, const char* path, const char* body=NULL);

#endif
