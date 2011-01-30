#include <QDebug>
#include <QApplication>
#include <QMenu>
#include "systemtrayicon.h"
#include "jsapi.h"
#include "dataloader.h"
#include "debugger.h"

static SystemTrayIconApp* g_app = 0;

SystemTrayIconApp::SystemTrayIconApp(QObject *parent)
	: App(parent)
{
}

QByteArray SystemTrayIconApp::processCall(const QString& command, const QVariantMap& param)
{
	QSystemTrayIcon* icon;

	if (command == "create") {
		icon = new QSystemTrayIcon();
		connect(icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
				this, SLOT(notifyClicked(QSystemTrayIcon::ActivationReason)));
		return QString("{\"type\":\"SystemTrayIcon\",\"id\":\"%1\"}")
			.arg(getId(icon)).toLatin1();
	}

	
	QString id = param.value("id").toString();
	icon = static_cast<QSystemTrayIcon *>(getInstance(id));
	if (!icon) {
		return "{\"err\":\"invalid id\"}";
	}

	if (command == "destroy") {
		delete icon;
		return "null";
	} else if (command == "load") {
		DataLoader *loader = new DataLoader(icon);
		connect(loader, SIGNAL(got(QByteArray&)),
				this, SLOT(iconFetched(QByteArray&)));
		connect(loader, SIGNAL(got(QByteArray&)),
				loader, SLOT(deleteLater()));
		loader->load(param["url"].toString());
		return "null";
	} else if (command == "setContextMenu") {
		QString menu_id = param.value("menuId").toString();
		QMenu* menu = static_cast<QMenu *>(getInstance(menu_id));
		if (!menu) {
			return "{\"err\":\"invalid menu id\"}";
		}
		icon->setContextMenu(menu);
		return "null";
	}

	return "{\"err\":\"invalid command\"}";
}

void SystemTrayIconApp::iconFetched(QByteArray &data)
{
	QSystemTrayIcon *icon = static_cast<QSystemTrayIcon *>(sender()->parent());

	QPixmap pixmap;
	pixmap.loadFromData(data);

	icon->setIcon(QIcon(pixmap));
	icon->show();
}

void SystemTrayIconApp::notifyClicked(QSystemTrayIcon::ActivationReason reason)
{
	QSystemTrayIcon *icon = static_cast<QSystemTrayIcon *>(sender());
	QString channel = QString("systray.SystemTrayIcon.%1.").arg(getId(icon));

	switch (reason) {
		case QSystemTrayIcon::Trigger:
			onering_publish(qPrintable(channel+"click"), "null");
			break;
		case QSystemTrayIcon::Context:
			onering_publish(qPrintable(channel+"rightclick"), "null");
			break;
		case QSystemTrayIcon::DoubleClick:
			onering_publish(qPrintable(channel+"doubleclick"), "null");
			break;
		default:
			;
	}
}

static onering_response_handle_t app(const char* appname, const char* method, 
		const char* path, const char* body,
		const char** response, int* response_len)
{
	if (!g_app) {
		g_app = new SystemTrayIconApp();
	}

	return g_app->processRequest(appname, method, path, body, response, response_len);
}


static void app_free_response(const char* appname, onering_response_handle_t response_handle)
{
	g_app->freeResponse(appname, response_handle);
}

void register_systray_app(const char* appname)
{
	onering_register_app(appname, &app, &app_free_response);
}
