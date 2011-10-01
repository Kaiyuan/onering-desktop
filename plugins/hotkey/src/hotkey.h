#ifndef HOTKEY_H
#define HOTKEY_H

#include <QByteArray>
#include <onering.h>
#include "qxtglobalshortcut.h"
#include "app.h"

#ifdef __cplusplus
extern "C" {
#endif
ONERING_EXPORT const char* require_onering_version();
ONERING_EXPORT const char* get_onering_appname();
ONERING_EXPORT int register_onering_plugin(const char* appname);
ONERING_EXPORT void set_onering_helpers(onering_helpers_t* helpers);
#ifdef __cplusplus
}
#endif

void* app(const char* appname, const char* method, const char* path, const char* body, int body_len, const char** response, int* response_len);
void app_free_response(const char* appname, void* response);

class HotKeyApp : public App
{
public:
	static HotKeyApp& getApp(const QString& appname);
	QByteArray processCall(const QString& path, const QVariantMap& param);

private:
	HotKeyApp(const QString& appname);
	HotKeyApp(HotKeyApp const&);
	void operator=(HotKeyApp const&);

private slots:
	void activated();
};
#endif
