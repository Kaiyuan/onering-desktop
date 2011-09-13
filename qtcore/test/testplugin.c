#include <string.h>
#include <onering.h>

onering_helpers_t* h = NULL;

ONERING_EXPORT const char* require_onering_version()
{
	return "QtCore 1.0";
}

ONERING_EXPORT const char* get_onering_appname()
{
	return "testplugin";
}

void* app(const char* appname, const char* method, const char* path, const char* body, int body_len, const char**response, int* response_len)
{
	*response = "Hello Test Plugin";
	*response_len = strlen(*response);
	return 0;
}

void free_response(const char* appname, void* handler)
{
}

ONERING_EXPORT int register_onering_plugin(const char* appname)
{
	if (!h) {
		return -2;
	}

	return h->register_app(appname, &app, &free_response);
}

ONERING_EXPORT void set_onering_helpers(onering_helpers_t* helpers)
{
	h = helpers;
}
