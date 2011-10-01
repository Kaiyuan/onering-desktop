#include <QDebug>
#include <QString>
#include <QHash>
#include <QPair>
#include <QUrl>
#include <string.h>
#include <assert.h>
#include <onering.h>
#include "app.h"
#include "json.h"

QHash<QString, QPair<onering_app_func_t, onering_free_response_func_t> > g_apps;

int onering_register_app(const char* appname, onering_app_func_t app_func, onering_free_response_func_t free_response_func)
{
	if (strlen(appname) > ONERING_MAX_APPNAME_LEN) {
		return ONERING_ERR_INVALID_APPNAME;
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

int get_app(const char* appname, onering_app_func_t* app, onering_free_response_func_t* free_respnose)
{
	if (!is_appname_registered(appname)) {
		return -1;
	}

	*app = g_apps[appname].first;
	*free_respnose = g_apps[appname].second;
	return 0;
}

QByteArray call_app(const QString &method, const QUrl &url, const QString &body)
{
	QString appname = url.host();
	const char * response;
	int response_len;
	void* response_handle;
	QByteArray retval;

	if (is_appname_registered(appname)) {
		QByteArray surl = url.toString().toUtf8();
		QByteArray path_query = url.path().toUtf8();
		if (url.hasQuery()) {
			path_query += surl.mid(surl.indexOf('?'));
		}

		QByteArray qbody = body.toUtf8();
		response_handle = g_apps[appname].first(qPrintable(appname), qPrintable(method), path_query, qbody.constData(), qbody.size(), &response, &response_len);
		retval.append(response, response_len);
		// free response
		g_apps[appname].second(qPrintable(appname), response_handle);
	}

	return retval;
}

QByteArray call_app_body(const QString &method, const QUrl &url, const QString &body)
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

App::App(const QString& appname, QObject* parent)
	: QObject(parent),
	  appname(appname)
{
}

void* App::processRequest(const char* appname,
		const char* method, const QString& path, const QByteArray& body,
	       	const char** response, int* response_len)
{
	Q_UNUSED(appname);
	Q_UNUSED(method);

	assert(path.startsWith("/"));

	QByteArray* res = new QByteArray();
	
	QVariantMap param = Json::parse(QString::fromUtf8(body)).toMap();
	*res = processCall(path.mid(1), param);

	*response = res->constData();
	*response_len = res->size();
	return res;
}

void App::freeResponse(const char* appname, void* handle)
{
	Q_UNUSED(appname);

	delete static_cast<QByteArray*>(handle);
}

QString App::generateObjectId(void* obj)
{
	return QString::number(reinterpret_cast<unsigned long>(obj), 16);
}


QString App::getId(QObject* obj)
{
	connect(obj, SIGNAL(destroyed(QObject*)),
			this, SLOT(instanceDestroyed(QObject*)),
			Qt::UniqueConnection);
	return generateObjectId(obj);
}

QString App::getId(void* obj)
{
	return generateObjectId(obj);
}

void* App::getInstance(const QString& id)
{
	bool ok;
	return reinterpret_cast<void *>(id.toULong(&ok, 16));
}

void App::instanceDestroyed(QObject* obj)
{
	qDebug() << obj << "deleted";
}

void App::publishEvent(const QString& type, void* sender, const QString& event)
{
	publishEvent(type, sender, event, "null");
}

void App::publishEvent(const QString& type, void* sender, const QString& event, QEvent* e)
{
	QString data = QString("{\"event_id\":\"%1\"}").arg(getId(e));
	publishEvent(type, sender, event, data);
}


void App::publishEvent(const QString& type, void* sender, const QString& event, const QString& data)
{
	onering_publish(qPrintable(QString("%1.%2.%3.%4")
				.arg(appname, type, getId(sender), event)),
			qPrintable(data));
}
