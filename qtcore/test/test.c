#include <string.h>
#include <stdio.h>
#include <onering.h>

onering_response_handle_t test_app(const char* appname, const char* method, const char* path, const char* body, const char** response, int* response_len)
{
	if (strcmp(path, "/init") == 0) {
		*response = "{\"width\": 400, \"height\": 300, \"url\": \"/\"}";
	} else {
		*response = "<html><head><script src='onering://onering/onering.js'></script></head><body><p>Hello, World!</p></body></html>";
	}
	*response_len = strlen(*response);
	return (onering_response_handle_t)0;
}

void free_response(const char* appname, onering_response_handle_t response_handle)
{
}

int main()
{
	printf("load %d plugins\n", onering_load_plugins("plugins"));
	onering_register_app("test", &test_app, &free_response);
/*        onering_loop("test");*/
	return 0;
}
