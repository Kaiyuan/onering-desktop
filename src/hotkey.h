#ifndef HOTKEY_H
#define HOTKEY_H

#include "qxtglobalshortcut.h"
#include "app.h"

class HotKeyApp : public App
{
	Q_OBJECT

public:
	HotKeyApp(const QString& appname, QObject *parent=0);

private slots:
	void activated();

private:
	QByteArray processCall(const QString& command, const QVariantMap& param);
};

void register_hotkey_app(const char* appname);

#endif
