#include <QDebug>
#include <QApplication>
#include "networkaccessmanager.h"
#include "systemtrayicon.h"
#include "jsapi.h"

SystemTrayIcon::SystemTrayIcon(QObject *parent)
	: QSystemTrayIcon(parent)
{
	setContextMenu(0);
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(emitActivatedEvent(QSystemTrayIcon::ActivationReason)));
	connect(qApp, SIGNAL(aboutToQuit()),
		this, SLOT(deleteLater()));
}

void SystemTrayIcon::load(const QUrl &url)
{
	NetworkAccessManager *manager = new NetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(iconFetched(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void SystemTrayIcon::iconFetched(QNetworkReply *reply)
{
	QPixmap pixmap;
	QByteArray data = reply->readAll();
	qDebug() << "icon data size:" << data.size();
	pixmap.loadFromData(data);
	reply->manager()->deleteLater();

	this->setIcon(QIcon(pixmap));
	this->show();
}

void SystemTrayIcon::emitActivatedEvent(QSystemTrayIcon::ActivationReason reason)
{
	static const QString click("click");
	static const QString doubleclick("doubleclick");
	static const QString rightclick("rightclick");

	switch (reason) {
		case QSystemTrayIcon::Trigger:
			emit activated(click);
			break;
		case QSystemTrayIcon::Context:
			emit activated(rightclick);
			break;
		case QSystemTrayIcon::DoubleClick:
			emit activated(doubleclick);
			break;
		default:
			;
	}
}
