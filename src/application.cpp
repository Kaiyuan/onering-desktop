#include <QDir>
#include <QDebug>
#include <QWebSettings>
#include <QWebSecurityOrigin>
#include <QScriptEngine>
#include <QVariantMap>
#include <QDesktopServices>
#include <onering.h>
#include "application.h"
#include "oneringapp.h"
#include "app.h"
#include "oneringview.h"

static QUrl getAbsUrl(const QString &url, const QString &appname)
{
	QUrl u(url);

	if (u.isRelative()) {
		u.setScheme("onering");
		u.setHost(appname);
	}
	return u;
}

Application::Application()
{
}

int Application::load(const char* appname)
{
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

	QString s;
	QUrl url;

	QString real_appname = props.value("appname", appname).toString();
	qDebug() << "appname:" << real_appname;
	QCoreApplication::setApplicationName(real_appname);
	QString appdir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
	qDebug() << "data location:" << appdir;
	settings->setLocalStoragePath(appdir);

	s = props.value("icon").toString();
	if (!s.isEmpty()) {
		url = getAbsUrl(s, appname);
	}

	int width = props["width"].toInt();
	int height = props["height"].toInt();
	s = props["url"].toString();
	if (s.isEmpty()) {
		s = "/";
	}
	url = getAbsUrl(s, appname);

	OneRingView *window = new OneRingView(url, width, height, props);
	window->show();
	return 0;
}
