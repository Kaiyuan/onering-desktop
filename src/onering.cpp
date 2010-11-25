#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QCoreApplication>
#include <QScriptEngine>
#include <QWebSettings>
#include <QWebSecurityOrigin>
#include <string.h>
#include <onering.h>
#include "oneringview.h"
#include "app.h"
#include "oneringapp.h"
#include "dataloader.h"
#include "application.h"

int onering_loop(const char* appname)
{
	int argc = 1;
	char name[ONERING_MAX_APPNAME_LEN+1];
	char *argv[] = {name};

	strncpy(name, appname, ONERING_MAX_APPNAME_LEN);
	name[ONERING_MAX_APPNAME_LEN] = '\0';

	Application app(argc, argv);
	int retval = app.load(appname);
	if (retval)
		return retval;
	return app.exec();
}

void onering_publish(const char* channel, const char* msg)
{
	Application *app = static_cast<Application *>(qApp);
	QEvent *event = new PublishEvent(channel, msg);
	QCoreApplication::postEvent(&(app->pubsubhub), event);
}
