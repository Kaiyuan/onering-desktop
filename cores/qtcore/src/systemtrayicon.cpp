#include <QDebug>
#include <QApplication>
#include <QMenu>
#include "systemtrayicon.h"
#include "jsapi.h"
#include "dataloader.h"
#include "debugger.h"

static SystemTrayIconApp* g_app = 0;

SystemTrayIconApp::SystemTrayIconApp(const QString& appname, QObject *parent)
	: App(appname, parent)
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
			publishEvent("SystemTrayIcon", icon, "click");
			break;
		case QSystemTrayIcon::Context:
			publishEvent("SystemTrayIcon", icon, "rightclick");
			break;
		case QSystemTrayIcon::DoubleClick:
			publishEvent("SystemTrayIcon", icon, "doubleclick");
			break;
		default:
			;
	}
}

static void* app(const char* appname, const char* method, 
		const char* path, const char* body, int body_len,
		const char** response, int* response_len)
{
	if (!g_app) {
		g_app = new SystemTrayIconApp(appname);
	}

	return g_app->processRequest(appname, method, path, QByteArray(body, body_len), response, response_len);
}


static void app_free_response(const char* appname, void* response_handle)
{
	g_app->freeResponse(appname, response_handle);
}

void register_systray_app(const char* appname)
{
	onering_register_app(appname, &app, &app_free_response);
}
