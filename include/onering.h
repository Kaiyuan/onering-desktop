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

#define ONERING_OK 0
#define ONERING_ERR_INVALID_APPNAME -1
#define ONERING_ERR_NO_HELPER_FUNCTION -2

/* Return the version number of the core.
 * e.g. "OneRingQtCore 1.0.0" */
ONERING_EXPORT const char* onering_version();

/* Load all OneRingPlugins under dir.
 * Returns number of loaded plugins. */
ONERING_EXPORT int onering_load_plugins(const char* dir);

/* Start app */
ONERING_EXPORT int onering_loop(const char* appname);

/* Call a app */
typedef void* (*onering_app_func_t) (
		const char* appname,
		const char* method,
		const char* path,
		const char* body,
		int body_len,
		/*OUT*/
		const char** response,
		int* response_len);
ONERING_EXPORT void* onering_call_app(
		const char* appname,
		const char* method,
		const char* path,
		const char* body,
		int body_len,
		/*OUT*/
		const char** response,
		int* response_len);

/* Release a response */
typedef void (*onering_free_response_func_t) (
		const char* appname,
		void* response_handle);
ONERING_EXPORT void onering_free_response(
		const char* appname,
		void* response_handle);

/* Register a app with the name appname.
 * Returns non-zero if registration failed. */
typedef int (*onering_register_app_func_t) (
		const char* appname,
		onering_app_func_t app_func,
		onering_free_response_func_t free_response_func);
ONERING_EXPORT int onering_register_app(
		const char* appname,
		onering_app_func_t app_func,
		onering_free_response_func_t free_response_func);

/* Subscribe a channel.
 * When messages are published into this channel, the
 * app's method will be called, with message as body.  The response will be
 * freed automatically after the calling. */
typedef void (*onering_subscribe_func_t) (
		const char* channel,
		const char* callback_appname,
		const char* callback_method);
ONERING_EXPORT void onering_subscribe(
		const char* channel,
		const char* callback_appname,
		const char* callback_method);

/* Unsubscribe the subscription to a channel */
typedef void (*onering_unsubscribe_func_t) (
		const char* channel,
		const char* callback_appname,
		const char* callback_method);
ONERING_EXPORT void onering_unsubscribe(
		const char* channel,
		const char* callback_appname,
		const char* callback_method);

/* Publish a message into a channel */
typedef void (*onering_publish_func_t) (
		const char* channel,
		const char* msg);
ONERING_EXPORT void onering_publish(
		const char* channel,
		const char* msg);


/* Functions should be exported from apps */

typedef struct {
	onering_app_func_t call_app;
	onering_free_response_func_t free_response;
	onering_register_app_func_t register_app;
	onering_subscribe_func_t subscribe;
	onering_unsubscribe_func_t unsubscribe;
	onering_publish_func_t publish;
} onering_helpers_t;

/* const char* require_onering_version() */
typedef const char* (*require_onering_version_func_t) ();
/* const char* get_onering_appname() */
typedef const char* (*get_onering_appname_func_t) ();
/* int register_onering_plugin(const char* appname) */
typedef int (*register_onering_plugin_func_t) (const char* appname);
/* void set_onering_helpers(onering_helpers_t* helpers) */
typedef void (*set_onering_helpers_func_t) (onering_helpers_t* helpers);


#ifdef __cplusplus
}
#endif

#endif
