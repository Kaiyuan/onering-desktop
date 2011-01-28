#include <QDebug>
#include <QString>
#include <QHash>
#include <QPair>
#include <QUrl>
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

QByteArray call_app(const char* method, const QUrl &url, const char* body=NULL)
{
	QString appname = url.host();
	const char * response;
	int response_len;
	onering_response_handle_t response_handle;
	QByteArray retval;

	if (is_appname_registered(appname)) {
		QByteArray surl = url.toString().toUtf8();
		QByteArray path_query = url.path().toUtf8();
		if (url.hasQuery()) {
			path_query += surl.mid(surl.indexOf('?'));
		}

		response_handle = g_apps[appname].first(qPrintable(appname), method, path_query, body, &response, &response_len);
		retval.append(response, response_len);
		// free response
		g_apps[appname].second(qPrintable(appname), response_handle);
	}

	return retval;
}

QByteArray call_app_body(const char* method, const QUrl &url, const char* body=NULL)
{
	QByteArray response = call_app(method, url, body);
	
	int index;
	if ((index = response.indexOf("\r\n")) < 0) {
		// not header/body seperator found.
		// consider the whole response is body
		return response;
	}
	QByteArray line = response.left(index);
	if (line.indexOf("200") < 0) {
		qDebug() << "error status line:" << line;
		return "";
	}

	if ((index = response.indexOf("\r\n\r\n")) < 0) {
		qDebug() << "Body not found:" << response;
		return "";
	}
	response.remove(0, index+4);
	return response;
}

