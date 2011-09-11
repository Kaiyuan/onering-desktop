#include <QDebug>
#include <QByteArray>
#include <onering.h>
#include "hotkey.h"
#include "json.h"

ONERING_EXPORT const char* require_onering_version()
{
	return "OneRingQtCore 1.0";
}

ONERING_EXPORT const char* get_onering_appname()
{
	return "hotkey";
}

ONERING_EXPORT int register_onering_plugin(const char* appname)
{
	return onering_register_app(appname, &app, &app_free_response);
}

onering_response_handle_t app(const char* appname, const char* method, const char* path, const char* body, const char** response, int* response_len)
{
	return HotKeyApp::getApp(appname).processRequest(appname, method, path, body, response, response_len);
}

void app_free_response(const char* appname, onering_response_handle_t response_handle)
{
	HotKeyApp::getApp(appname).freeResponse(appname, response_handle);
}

HotKeyApp::HotKeyApp(const QString& appname)
	: App(appname)
{
}

HotKeyApp& HotKeyApp::getApp(const QString& appname)
{
	static HotKeyApp app(appname);
	return app;
}

QByteArray HotKeyApp::processCall(const QString& command, const QVariantMap& param)
{
	QxtGlobalShortcut* shortcut;

	if (command == "create") {
		shortcut = new QxtGlobalShortcut(QKeySequence(param["shortcut"].toString()));
		connect(shortcut, SIGNAL(activated()),
				this, SLOT(activated()));
		return QString("{\"type\":\"HotKey\",\"id\":\"%1\"}").arg(getId(shortcut)).toLatin1();
	}

	shortcut = static_cast<QxtGlobalShortcut *>(getInstance(param["id"].toString()));
	if (!shortcut) {
		return "{\"err\":\"invalid id\"}";
	}

	if (command == "destroy") {
		delete shortcut;
		return "null";
	}

	return "{\"err\":\"invalid command\"}";
}

void HotKeyApp::activated()
{
	qDebug() << "activated";
	publishEvent("HotKey", sender(), "activated");
}

