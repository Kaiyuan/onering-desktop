#ifndef HOTKEY_H
#define HOTKEY_H

#include <QByteArray>
#include <onering.h>
#include "qxtglobalshortcut.h"
#include "app.h"

onering_response_handle_t app(const char* appname, const char* method, const char* path, const char* body, const char** response, int* response_len);
void app_free_response(const char* appname, onering_response_handle_t response);

class HotKeyApp : public App
{
public:
	static HotKeyApp& getApp(const QString& appname);
	QByteArray processCall(const QString& path, const QVariantMap& param);

private slots:
	HotKeyApp(const QString& appname);
	HotKeyApp(HotKeyApp const&);
	void operator=(HotKeyApp const&);

	void activated();
};
#endif
