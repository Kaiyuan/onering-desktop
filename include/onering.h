#ifndef _ONERING_H
#define _ONERING_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_APPNAME_LEN 63

#if defined(ONERING_LIBRARY) && defined(Q_WS_WIN)
#  define ONERING_EXPORT __declspec( dllexport )
#else
#  define ONERING_EXPORT
#endif

typedef void (*onering_app_func_t) (const char* method, const char* path, const char* body,
		/*OUT*/ const char** response, int* response_len);

/* Return -1 means strlen(appname) > MAX_APPNAME_LEN */
ONERING_EXPORT int onering_register_app(const char* appname, onering_app_func_t app_func);

ONERING_EXPORT int onering_loop(const char* appname);

#ifdef __cplusplus
}
#endif

#endif
