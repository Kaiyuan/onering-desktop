#ifndef ONERING_H
#define ONERING_H

#ifdef __cplusplus
extern "C" {
#endif

#define ONERING_MAX_APPNAME_LEN 63

#if defined(ONERING_LIBRARY) && defined(Q_WS_WIN)
#  define ONERING_EXPORT __declspec( dllexport )
#else
#  define ONERING_EXPORT
#endif

typedef long onering_response_handle_t;

typedef onering_response_handle_t (*onering_app_func_t) (const char* appname, const char* method, const char* path, const char* body,
		/*OUT*/ const char** response, int* response_len);
typedef void (*onering_free_response_func_t) (const char* appname, onering_response_handle_t response_handle);

/* Register a app with the name appname.
 * Register Return -1 means strlen(appname) > MAX_APPNAME_LEN */
ONERING_EXPORT int onering_register_app(
		const char* appname,
		onering_app_func_t app_func,
		onering_free_response_func_t free_response_func);

/* Start app */
ONERING_EXPORT int onering_loop(const char* appname);

/* Return the version number of the core.
 * e.g. "OneRingQtCore 1.0.0" */
ONERING_EXPORT const char* onering_version();

/* Load all OneRingPlugins under dir.
 * Returns number of loaded plugins. */
ONERING_EXPORT int onering_load_plugins(const char* dir);

/* Call a app */
ONERING_EXPORT onering_response_handle_t onering_call_app(
		const char* appname,
		const char* method,
		const char* body,
		int body_len,
		char** response,
		int* response_len);

/* Release a response */
ONERING_EXPORT void onering_free_response(
		const char* appname,
		onering_response_handle_t response_handle);

/* Subscribe a channel.
 * When messages are published into this channel, the
 * app's method will be called, with message as body.  The response will be
 * freed automatically after the calling. */
ONERING_EXPORT void onering_subscribe(
		const char* channel,
		const char* callback_appname,
		const char* callback_method);

/* Unsubscribe the subscription to a channel */
ONERING_EXPORT void onering_unsubscribe(
		const char* channel,
		const char* callback_appname,
		const char* callback_method);

/* Publish a message into a channel */
ONERING_EXPORT void onering_publish(
		const char* channel,
		const char* msg);

#ifdef __cplusplus
}
#endif

#endif
