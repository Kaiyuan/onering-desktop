#include <string.h>
#include <stdio.h>
#include <onering.h>

static const char HTML[] = "\
<html><head> \
<script src='onering://onering/onering.js'></script> \
<script src='onering://hotkey/hotkey.js'></script> \
</head><body> \
<p>Press Alt+P to increase number.</p> \
<div id='x'>0</div>\
<script>\
var n = 0;\
ONERING.getCurrentWindow().showInspector();\
ONERING.HotKey.set('Ctrl+Alt+R', function(){\
		n = n + 1; \
		document.getElementById('x').innerHTML = n;\
		});\
</script>\
</body></html>";


void* test_app(const char* appname, const char* method, const char* path, const char* body, int body_len, const char** response, int* response_len)
{
	if (strcmp(path, "/init") == 0) {
		*response = "{\"width\": 400, \"height\": 300, \"url\": \"/\"}";
	} else {
		*response = HTML;
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

	return onering_loop("test");
}
