#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QCoreApplication>
#include <QScriptEngine>
#include <QWebSettings>
#include <QWebSecurityOrigin>
#include <onering.h>
#include "oneringview.h"
#include "app.h"
#include "oneringapp.h"

int onering_loop(const char* appname)
{
	int argc = 1;
	char *argv[] = {(char*)appname};
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName(appname);

	QDir plugins_dir = QCoreApplication::applicationDirPath();
	plugins_dir.cd("plugins");
	qDebug() << "plugins dir:" << plugins_dir;
	QCoreApplication::setLibraryPaths(QStringList(plugins_dir.path()));

	QWebSettings *settings = QWebSettings::globalSettings();
	settings->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	settings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
	settings->setAttribute(QWebSettings::LocalStorageEnabled, true);
	settings->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, true);
	settings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
#ifdef DEBUG
	settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif

	QString appdir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
	settings->setLocalStoragePath(appdir);

	QWebSecurityOrigin::addLocalScheme("onering");

	if (onering_app_init())
		return -1;
	onering_register_app("onering", &onering_app);

	QByteArray response = call_app(appname, "GET", "/init");

	// quick check for body
	int index;
	if ((index = response.indexOf("\r\n\r\n")) < 0) {
		qDebug() << "Body not found:" << response;
		return -2;
	}
	response.remove(0, index+4);

	QScriptValue sc;
	QScriptEngine engine;
	sc = engine.evaluate("("+response+")");

	int width=0, height=0;
	QString title, url;
	width = sc.property("width").toInt32();
	height = sc.property("height").toInt32();
	title = sc.property("title").toString();
	url = sc.property("url").isValid() ? sc.property("url").toString() : "/";
	if (url[0] == '/') {
		url = QString("onering://") + appname + url;
	}

	OneRingView *window = new OneRingView();
	window->load(url);

	window->resize(width, height);
	window->setWindowTitle(title);
	window->show();
	return app.exec();
}
