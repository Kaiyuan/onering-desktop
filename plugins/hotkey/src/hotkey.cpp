#include <QDebug>
#include <QByteArray>
#include <onering.h>
#include "hotkey.h"
#include "json.h"

static const char JS[] = " \
ONERING.HotKey = function() { \n\
}; \n\
ONERING.HotKey.prototype = (new ONERING.Base()).extend({ \n\
		appname: \"hotkey\", \n\
		type: \"HotKey\", \n\
		destroy: function() { \n\
			return this._call(\"destroy\"); \n\
		}, \n\
	}); \n\
ONERING.HotKey.instances = {}; \n\
ONERING.HotKey.create = function(shortcut) { \n\
	var hotkey = new ONERING.HotKey(); \n\
	hotkey.id = (hotkey._create(\"create\", {shortcut: shortcut})).id; \n\
	ONERING.HotKey.instances[shortcut] = hotkey; \n\
	return hotkey; \n\
}; \n\
ONERING.HotKey.set = function(shortcut, callback) { \n\
	var hotkey = ONERING.HotKey.instances[shortcut] || new ONERING.HotKey.create(shortcut); \n\
	hotkey.bind(\"activated\", callback); \n\
}; \n\
ONERING.HotKey.clear = function(shortcut) { \n\
	var hotkey = ONERING.HotKey.instances[shortcut]; \n\
	if (hotkey !== undefined) { \n\
		hotkey.destroy(); \n\
		delete ONERING.HotKey.instances[shortcut]; \n\
	} \n\
}; \n\
ONERING.setHotKey = ONERING.HotKey.set; \n\
ONERING.clearHotKey = ONERING.HotKey.clear; \n\
 \n\
window.addEventListener('unload', function() { \n\
		for (var shortcut in ONERING.HotKey.instances) { \n\
			ONERING.HotKey.clear(shortcut); \n\
		} \n\
	}); \n\
";


const char* require_onering_version()
{
	return "QtCore 1.0";
}

const char* get_onering_appname()
{
	return "hotkey";
}

int register_onering_plugin(const char* appname)
{
	onering_helpers_t* h = HotKeyApp::getApp(appname).getHelpers();
	if (!h || !(h->register_app)) {
		return ONERING_ERR_NO_HELPER_FUNCTION;
	}
	return h->register_app(appname, &app, &app_free_response);
}

void set_onering_helpers(onering_helpers_t* helpers)
{
	HotKeyApp::setHelpers(helpers);
}

void* app(const char* appname, const char* method, const char* path, const char* body, int body_len, const char** response, int* response_len)
{
	return HotKeyApp::getApp(appname).processRequest(appname, method, path, QByteArray(body, body_len), response, response_len);
}

void app_free_response(const char* appname, void* handle)
{
	HotKeyApp::getApp(appname).freeResponse(appname, handle);
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

	if (command == "hotkey.js") {
		return JS;
	}

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

