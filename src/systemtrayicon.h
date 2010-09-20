#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QNetworkReply>
#include "jsapi.h"

class SystemTrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SystemTrayIcon(QObject *parent=0);

	void load(const QString &url);

signals:
	void activated(const QString &event);

private slots:
	void iconFetched(QNetworkReply *reply);
	void emitActivatedEvent(QSystemTrayIcon::ActivationReason reason);
};

#endif
