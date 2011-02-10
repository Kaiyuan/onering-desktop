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

void onering_publish(const char* channel, const char* msg)
{
	Application *app = static_cast<Application *>(qApp);
	if (app && !app->quiting) {
		app->pubsubhub.publish(channel, msg);
	}
}
