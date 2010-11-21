#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QByteArray>
#include "jsapi.h"

class SystemTrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SystemTrayIcon(QObject *parent=0);

	void load(const QUrl &url);

signals:
	void activated(const QString &event);

private slots:
	void iconFetched(QByteArray &data);
	void emitActivatedEvent(QSystemTrayIcon::ActivationReason reason);
};

#endif
