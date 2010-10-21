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

#ifdef Q_OS_MAC
	QDir plugins_dir = QCoreApplication::applicationDirPath();
	if (!plugins_dir.path().startsWith("/System/")) {
		plugins_dir.cd("../plugins");
		QCoreApplication::setLibraryPaths(QStringList(plugins_dir.path()));
	}
#endif
	qDebug() << "plugins dir:" << QCoreApplication::libraryPaths();

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

	QScriptEngine engine;
	QVariantMap props = engine.evaluate("("+response+")").toVariant().toMap();

	int width = props["width"].toInt();
	int height = props["height"].toInt();
	QString url = props["url"].toString();
	if (url.isEmpty()) {
		url = "/";
	}
	if (url[0] == '/') {
		url = QString("onering://") + appname + url;
	}

	OneRingView *window = new OneRingView(url, width, height, props);
	window->show();
	return app.exec();
}

