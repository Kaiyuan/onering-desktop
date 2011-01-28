#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QByteArray>
#include "jsapi.h"
#include "menu.h"

class SystemTrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SystemTrayIcon(QObject *parent=0);

	Q_INVOKABLE void load(const QString &url);
	Q_INVOKABLE QVariantMap getGeometry(void);
	Q_INVOKABLE void setContextMenu(const QString& menu_id);

signals:
	void click();
	void doubleclick();
	void rightclick();

private slots:
	void iconFetched(QByteArray &data);
	void emitActivatedEvent(QSystemTrayIcon::ActivationReason reason);
};

#endif
