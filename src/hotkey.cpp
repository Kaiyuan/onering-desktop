#include "hotkey.h"

static HotKeyApp* g_app = 0;

HotKeyApp::HotKeyApp(QObject *parent)
	: App(parent)
{
}

QByteArray HotKeyApp::processCall(const QString& command, const QVariantMap& param)
{
	return "{\"err\":\"invalid command\"}";
}

static onering_response_handle_t app(const char* appname, const char* method, 
		const char* path, const char* body,
		const char** response, int* response_len)
{
	if (!g_app) {
		g_app = new HotKeyApp();
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
