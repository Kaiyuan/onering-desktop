#include <string.h>
#include <stdio.h>
#include <onering.h>

void* test_app(const char* appname, const char* method, const char* path, const char* body, int body_len, const char** response, int* response_len)
{
	if (strcmp(path, "/init") == 0) {
		*response = "{\"width\": 400, \"height\": 300, \"url\": \"/\"}";
	} else {
		*response = "<html><head><script src='onering://onering/onering.js'></script></head><body><p>Hello, World!</p></body></html>";
	}
	*response_len = strlen(*response);
	return 0;
}

void free_response(const char* appname, void* response_handle)
{
}

int main()
{
	printf("load %d plugins\n", onering_load_plugins("plugins"));
	onering_register_app("test", &test_app, &free_response);

	const char* response;
	int response_len;
	void* handler;

	printf("GET onering://testplugin/ ->\n");
	onering_call_app("testplugin", "GET", "/", NULL, 0, &response, &response_len);
	printf("%s\n", response);
	onering_free_response("testplugin", handler);

	printf("GET onering://testplugin2/ ->\n");
	handler = onering_call_app("testplugin2", "GET", "/", NULL, 0, &response, &response_len);
	printf("%s\n", response);
	onering_free_response("testplugin2", handler);
	
	return 0;
}
