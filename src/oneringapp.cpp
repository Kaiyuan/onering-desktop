#include <QDebug>
#include <QFile>
#include "oneringapp.h"

QByteArray onering_js;  // global to hold char * reference

int onering_app_init()
{
	QFile file(":/js/onering.js");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return -1;
	}
	onering_js = file.readAll();
	onering_js.prepend("HTTP/1.0 200 OK\r\nContent-Type: text/javascript\r\n\r\n");
	return 0;
}

void onering_app(const char* method, const char* path, const char* body, const char **response, int *response_len)
{
	if (strcmp(method, "GET") == 0 && strcmp(path, "/onering.js") == 0) {
		*response = onering_js.constData();
		*response_len = onering_js.size();
	} else {
		// TODO: 404
		*response = "";
		*response_len = 0;
	}
}
