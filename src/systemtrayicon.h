#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QByteArray>
#include "app.h"

class SystemTrayIconApp : public App
{
Q_OBJECT

public:
	SystemTrayIconApp(QObject* parent=0);

private:
	QByteArray processCall(const QString& command, const QVariantMap& param);

private slots:
	void iconFetched(QByteArray &data);
	void notifyClicked(QSystemTrayIcon::ActivationReason reason);
};

void register_systray_app(const char* appname);

#endif
