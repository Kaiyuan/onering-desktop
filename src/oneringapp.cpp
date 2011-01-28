#include <QDebug>
#include <QFile>
#include <onering.h>
#include "oneringapp.h"
#include "debugger.h"

static QByteArray *onering_js = 0;  // global to hold char * reference

int onering_app_init()
{
	QFile file(":/js/onering.js");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return -1;
	}
	onering_js = new QByteArray(file.readAll());
	qDebug() << "=== NEW" << onering_js;
	onering_js->prepend("HTTP/1.0 200 OK\r\nContent-Type: text/javascript\r\n\r\n");
	return 0;
}

static onering_response_handle_t onering_app(const char *appname, const char* method, const char* path, const char* body, const char **response, int *response_len)
{
	if (strcmp(method, "GET") == 0 && strcmp(path, "/onering.js") == 0 && onering_js) {
		*response = onering_js->constData();
		*response_len = onering_js->size();
	} else {
		// TODO: 404
		*response = "";
		*response_len = 0;
	}

	return 0;
}

static void onering_app_free_response(const char *appname, onering_response_handle_t response_handle)
{
}

void register_onering_app(const char* appname)
{
	onering_register_app(appname, &onering_app, &onering_app_free_response);
}
