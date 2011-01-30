#ifndef HOTKEY_H
#define HOTKEY_H

#include "qxtglobalshortcut.h"
#include "app.h"

class HotKeyApp : public App
{
public:
	HotKeyApp(QObject *parent=0);

private:
	QByteArray processCall(const QString& command, const QVariantMap& param);
};

void register_hotkey_app(const char* appname);

#endif
