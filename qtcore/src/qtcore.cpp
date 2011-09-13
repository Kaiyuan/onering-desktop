#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QCoreApplication>
#include <QWebSettings>
#include <QWebSecurityOrigin>
#include <string.h>
#include <onering.h>
#include "oneringview.h"
#include "app.h"
#include "oneringapp.h"
#include "dataloader.h"
#include "application.h"
#include "publishevent.h"
#include "cocoainit.h"
#include "pluginloader.h"

// Implementations of APIs in oneirng.h:
//
// onering_register_app() implemented in app.cpp
//

int onering_loop(const char* appname)
{
	int argc = 1;
	char name[ONERING_MAX_APPNAME_LEN+1];
	char *argv[] = {name};

	strncpy(name, appname, ONERING_MAX_APPNAME_LEN);
	name[ONERING_MAX_APPNAME_LEN] = '\0';

#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
	CocoaInitializer cocoaInitializer;
#endif
#endif
	Application app(argc, argv);
	int retval = app.load(appname);
	if (retval)
		return retval;
	return app.exec();
}

const char* onering_version()
{
	return "QtCore 1.0.0";
}

int onering_load_plugins(const char* dir)
{
	return PluginLoader::loadDir(QDir(dir));
}

void* onering_call_app(const char* appname, const char* method, const char* path, const char* body, int body_len, const char** response, int* response_len)
{
	onering_app_func_t app;
	onering_free_response_func_t free_response;

	if (get_app(appname, &app, &free_response)) {
		*response = "404 Not Found";
		*response_len = strlen(*response);
		return NULL;
	}

	return app(appname, method, path, body, body_len, response, response_len);
}

void onering_free_response(const char* appname, void* response_handle)
{
	onering_app_func_t app;
	onering_free_response_func_t free_response;

	if (get_app(appname, &app, &free_response)) {
		return;
	}

	free_response(appname, response_handle);
}

void onering_subscribe(const char* channel, const char* callback_appname, const char* callback_method)
{
}

void onering_unsubscribe(const char* channel, const char* callback_appname, const char* callback_method)
{
}

void onering_publish(const char* channel, const char* msg)
{
	Application *app = static_cast<Application *>(qApp);
	if (app && !app->quiting) {
		app->pubsubhub.publish(channel, msg);
	}
}
