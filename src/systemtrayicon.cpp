#include <QDebug>
#include <QApplication>
#include "systemtrayicon.h"
#include "jsapi.h"
#include "dataloader.h"
#include "debugger.h"

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
	DataLoader *loader = new DataLoader(this);
	Debugger::traceObj(loader);
	connect(loader, SIGNAL(got(QByteArray&)),
		this, SLOT(iconFetched(QByteArray&)));
	connect(loader, SIGNAL(got(QByteArray&)),
		loader, SLOT(deleteLater()));
	loader->load(url);
}

void SystemTrayIcon::iconFetched(QByteArray &data)
{
	QPixmap pixmap;
	pixmap.loadFromData(data);

	setIcon(QIcon(pixmap));
	show();
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
