#include <QDebug>
#include <QFile>
#include <onering.h>
#include "oneringapp.h"
#include "debugger.h"

static OneRingApp* g_app = 0;

OneRingApp::OneRingApp(QObject *parent)
	: App(parent)
{
	QFile file(":/js/onering.js");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	_js = file.readAll();
	file.close();
}

QByteArray OneRingApp::processCall(const QString& command, const QVariantMap& param)
{
	if (command == "onering.js") {
		return _js;
	}

	return "{\"err\":\"invalid command\"}";
}

static onering_response_handle_t onering_app(const char *appname, const char* method, const char* path, const char* body, const char **response, int *response_len)
{
	if (!g_app) {
		g_app = new OneRingApp();
	}

	return g_app->processRequest(appname, method, path, body, response, response_len);
}

static void onering_app_free_response(const char *appname, onering_response_handle_t response_handle)
{
}

void register_onering_app(const char* appname)
{
	onering_register_app(appname, &onering_app, &onering_app_free_response);
}
