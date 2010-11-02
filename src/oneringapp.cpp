#include <QDebug>
#include <QFile>
#include "oneringapp.h"

static QByteArray *onering_js = 0;  // global to hold char * reference

int onering_app_init()
{
	QFile file(":/js/onering.js");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return -1;
	}
	onering_js = new QByteArray(file.readAll());
	onering_js->prepend("HTTP/1.0 200 OK\r\nContent-Type: text/javascript\r\n\r\n");
	return 0;
}

onering_response_handle_t onering_app(const char* method, const char* path, const char* body, const char **response, int *response_len)
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

void onering_app_free_response(onering_response_handle_t response_handle)
{
}
