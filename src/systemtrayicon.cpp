#include <QDebug>
#include <QApplication>
#include <QMenu>
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

void SystemTrayIcon::load(const QString &url)
{
	qDebug() << "SystemTrayIcon::load" << url;
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
	switch (reason) {
		case QSystemTrayIcon::Trigger:
			emit click();
			break;
		case QSystemTrayIcon::Context:
			emit rightclick();
			break;
		case QSystemTrayIcon::DoubleClick:
			emit doubleclick();
			break;
		default:
			;
	}
}

QVariantMap SystemTrayIcon::getGeometry()
{
	QVariantMap geo;
	QRect rect = geometry();

	geo["top"] = rect.top();
	geo["left"] = rect.left();
	geo["width"] = rect.width();
	geo["height"] = rect.height();
	return geo;
}

void SystemTrayIcon::setContextMenu(const QString &menu_id)
{
	bool ok;
	QSystemTrayIcon::setContextMenu(reinterpret_cast<QMenu *>(menu_id.toLong(&ok, 16)));
}
