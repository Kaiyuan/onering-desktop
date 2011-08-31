#include <QApplication>
#include <QDebug>
#include <QFile>
#include <onering.h>
#include "application.h"
#include "oneringapp.h"
#include "oneringview.h"
#include "debugger.h"

static OneRingApp* g_app = 0;

OneRingApp::OneRingApp(const QString& appname, QObject *parent)
	: App(appname, parent)
{
	QFile file(":/js/onering.js");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	_js = file.readAll();
	file.close();
	_js.prepend("HTTP/1.1 200 OK\r\nContent-Type: application/javascript\r\n\r\n");

#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
	connect(static_cast<Application *>(qApp), SIGNAL(dockIconClicked()),
			this, SLOT(dockIconClicked()));
#endif
#endif
}

QByteArray OneRingApp::processCall(const QString& command, const QVariantMap& param)
{
	if (command == "onering.js") {
		return _js;
	} else if (command == "Window.create") {
		OneRingView* window = createWindow(param);
		_windows.insert(window);
		connect(window, SIGNAL(destroyed(QObject *)),
				this, SLOT(windowDestroyed(QObject *)));
		window->show();
		return QString("{\"type\":\"Window\",\"id\":\"%1\"}")
			.arg(getId(window)).toLatin1();
	} else if (command.startsWith("Window.")) {
		QString id = param.value("id").toString();
		OneRingView* window = static_cast<OneRingView *>(getInstance(id));

		if (command == "Window.isAlive") {
			return _windows.contains(window) ? "true" : "false";
		} else if (!window) {
			return "{\"err\":\"invalid id\"}";
		} else if (command == "Window.showInspector") {
			window->showInspector();
			return "null";
		} else if (command == "Window.hide") {
			window->hide();
			return "null";
		} else if (command == "Window.show") {
			window->show();
			return "null";
		} else if (command == "Window.maximize") {
			window->showMaximized();
			return "null";
		} else if (command == "Window.showNormal") {
			window->showNormal();
			return "null";
		} else if (command == "Window.isMinimized") {
			return window->isMinimized() ? "true" : "false";
		} else if (command == "Window.activateWindow") {
			window->activateWindow();
			return "null";
		}
	} else if (command.startsWith("Application.")) {
		if (command == "Application.setQuitOnLastWindowClosed") {
			qApp->setQuitOnLastWindowClosed(param["quit"].toBool());
			return "null";
		} else if (command == "Application.exit") {
			qApp->quit();
			return "null";
		}
	} else if (command == "Event.preventDefault") {
		QString id = param.value("id").toString();
		QEvent* event = static_cast<QEvent *>(getInstance(id));
		if (!event) {
			return "{\"err\":\"invalid id\"}";
		}
		event->ignore();
		return "null";
	}

	return "{\"err\":\"invalid command\"}";
}

OneRingView* OneRingApp::createWindow(const QVariantMap& props)
{
	if (!g_app) {
		g_app = new OneRingApp("onering");
	}

	OneRingView* window = new OneRingView(props);
	connect(window, SIGNAL(eventOccurred(QEvent*, const QString&)),
			g_app, SLOT(windowEventOccurred(QEvent*, const QString&)));
	return window;
}

void OneRingApp::windowEventOccurred(QEvent* e, const QString& type)
{
	publishEvent("Window", sender(), type, e);
}

void OneRingApp::windowDestroyed(QObject *obj)
{
	_windows.remove(static_cast<OneRingView *>(obj));
}

#ifdef Q_WS_MAC
#ifdef QT_MAC_USE_COCOA
void OneRingApp::dockIconClicked()
{
	publishEvent("Application", 0, "dockIconClicked");
}
#endif
#endif

static onering_response_handle_t app(const char *appname, const char* method, const char* path, const char* body, const char **response, int *response_len)
{
	if (!g_app) {
		g_app = new OneRingApp(appname);
	}

	return g_app->processRequest(appname, method, path, body, response, response_len);
}

static void free_response(const char *appname, onering_response_handle_t response_handle)
{
	g_app->freeResponse(appname, response_handle);
}

void register_onering_app(const char* appname)
{
	onering_register_app(appname, &app, &free_response);
}


