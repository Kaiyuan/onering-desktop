#include "hotkey.h"

static HotKeyApp* g_app = 0;

HotKeyApp::HotKeyApp(const QString& appname, QObject *parent)
	: App(appname, parent)
{
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
	return "{\"err\":\"invalid command\"}";
}

void HotKeyApp::activated()
{
}

static onering_response_handle_t app(const char* appname, const char* method, 
		const char* path, const char* body,
		const char** response, int* response_len)
{
	if (!g_app) {
		g_app = new HotKeyApp(appname);
	}

	return g_app->processRequest(appname, method, path, body, response, response_len);
}


static void app_free_response(const char* appname, onering_response_handle_t response_handle)
{
	g_app->freeResponse(appname, response_handle);
}

void register_hotkey_app(const char* appname)
{
	onering_register_app(appname, &app, &app_free_response);
}
