#include <QDebug>
#include <QString>
#include <QHash>
#include <string.h>
#include <onering.h>

QHash<QString, onering_app_func_t> g_apps;

int onering_register_app(const char* appname, onering_app_func_t app_func)
{
	if (strlen(appname) > MAX_APPNAME_LEN) {
		return -1;
	}

	qDebug() << "register app:" << appname;
	g_apps[appname] = app_func;
	return 0;
}

int is_appname_registered(const QString &appname)
{
	return g_apps.contains(appname);
}

QByteArray call_app(const char* appname, const char* method, const char* path, const char* body=NULL)
{
	const char * response;
	int response_len;
	if (!is_appname_registered(appname)) {
		response = ""; /* TODO: 404 response */
		response_len = 0;
	} else {
		g_apps[appname](method, path, body, &response, &response_len);
	}
//        qDebug() << "call_app" << method << appname << path;
	return QByteArray(response, response_len);
}
