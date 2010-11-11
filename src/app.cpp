#include <QDebug>
#include <QString>
#include <QHash>
#include <QPair>
#include <string.h>
#include <onering.h>

QHash<QString, QPair<onering_app_func_t, onering_free_response_func_t> > g_apps;

int onering_register_app(const char* appname, onering_app_func_t app_func, onering_free_response_func_t free_response_func)
{
	if (strlen(appname) > ONERING_MAX_APPNAME_LEN) {
		return -1;
	}

	qDebug() << "register app:" << appname;
	QPair<onering_app_func_t, onering_free_response_func_t> funcs(app_func, free_response_func);
	g_apps[appname] = funcs;
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
	onering_response_handle_t response_handle;
	QByteArray retval;

	if (is_appname_registered(appname)) {
		response_handle = g_apps[appname].first(method, path, body, &response, &response_len);
		retval.append(response, response_len);
		// free response
		g_apps[appname].second(response_handle);
	}

	return retval;
}

QByteArray call_app_body(const char* appname, const char* method, const char* path, const char* body=NULL)
{
	QByteArray response = call_app(appname, method, path, body);
	
	int index;
	if ((index = response.indexOf("\r\n\r\n")) < 0) {
		qDebug() << "Body not found:" << response;
		return "";
	}
	response.remove(0, index+4);
	return response;
}

