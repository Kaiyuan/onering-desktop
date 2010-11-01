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
#include "dataloader.h"
#include "application.h"

int onering_loop(const char* appname)
{
	int argc = 1;
	char *argv[] = {(char*)appname};
	QApplication qapp(argc, argv);

	Application app;
	int retval = app.load(appname);
	if (retval)
		return retval;
	return qapp.exec();
}

